#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <algorithm>
#include <app/action/preset.hpp>
#include <app/ecs.hpp>
#include <app/preset/name.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>
#include <vector>

namespace VTX::UI::QT::Widget::Library
{
	/**
	 * @brief Base class for preset selectors to permit signals (not possible on templated class).
	 */
	class BasePresetSelector : public QGroupBox
	{
		Q_OBJECT

	  public:
		BasePresetSelector( QWidget * p_parent ) : QGroupBox( p_parent ) {}
		virtual ~BasePresetSelector() = default;

	  signals:
		/**
		 * @brief Signal emitted when the selected preset is changed from the widget.
		 */
		void presetChanged( const App::ECS::Entity );

		/**
		 * @brief Signal emitted when the current preset is updated from App.
		 */
		void currentPresetUpdated( const App::ECS::Entity );
	};

	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<typename P>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * p_parent ) : BasePresetSelector( p_parent )
		{
			auto * layout = new QVBoxLayout( this );
			setTitle( "Presets" );

			// layout->setContentsMargins( 0, 0, 0, 0 );

			_comboBox = new QComboBox( this );
			layout->addWidget( _comboBox );

			using namespace Action;

			auto * toolbar = new QToolBar( this );
			toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );
			// toolbar->setIconSize( QSize( 12, 12 ) );
			auto * aNew		  = Application::getAction<Preset::Add<P>>();
			auto * aDuplicate = Application::getAction<Preset::Duplicate<P>>();
			auto * aApply	  = Application::getAction<Preset::Apply<P>>();
			toolbar->addAction( aNew );
			toolbar->addAction( aDuplicate );

			// toolbar->addAction( Application::getAction<Preset::Delete<P>>() );
			toolbar->addAction( aApply );
			layout->addWidget( toolbar );

			_lineRename = new QLineEdit( this );
			layout->addWidget( _lineRename );

			connect(
				_comboBox,
				&QComboBox::currentIndexChanged,
				this,
				[ this ]( const int )
				{
					_lineRename->setText( _comboBox->currentText() );
					emit presetChanged( getCurrentPreset() );
				}
			);

			connect(
				aNew,
				&QAction::triggered,
				[ this ]() { App::ACTION().execute<App::Action::Preset::Add<P>>( std::nullopt, std::nullopt ); }
			);

			connect(
				aDuplicate,
				&QAction::triggered,
				[ this ]()
				{ App::ACTION().execute<App::Action::Preset::Duplicate<P>>( getCurrentPreset(), std::nullopt ); }
			);

			connect(
				aApply,
				&QAction::triggered,
				[ this ]() { App::ACTION().execute<App::Action::Preset::Apply<P>>( getCurrentPreset() ); }
			);

			connect(
				_lineRename,
				&QLineEdit::editingFinished,
				[ this ]()
				{
					App::ACTION().execute<App::Action::Preset::Rename<P>>(
						getCurrentPreset(), _lineRename->text().toStdString()
					);
				}
			);

			// Callbacks.
			auto & reg = App::REG();

			reg.on_construct<P>().template connect<&PresetSelector::_refreshComboBox>( this );
			reg.on_destroy<P>().template connect<&PresetSelector::_refreshComboBox>( this );
			reg.on_update<App::Preset::Name>().template connect<&PresetSelector::_onPresetNameUpdated>( this );
			reg.on_update<P>().template connect<&PresetSelector::_onUpdatePreset>( this );
		}

		virtual ~PresetSelector()
		{
			auto & reg = App::REG();
			reg.on_construct<P>().template disconnect<&PresetSelector::_refreshComboBox>( this );
			reg.on_destroy<P>().template disconnect<&PresetSelector::_refreshComboBox>( this );
			reg.on_update<App::Preset::Name>().template disconnect<&PresetSelector::_onPresetNameUpdated>( this );
			reg.on_update<P>().template disconnect<&PresetSelector::_onUpdatePreset>( this );
		}

		inline App::ECS::Entity getCurrentPreset() const { return _comboBox->currentData().value<App::ECS::Entity>(); }
		inline void				refresh() { _refreshComboBox( App::REG(), App::ECS::Entity {} ); }
		inline void				setCurrentPreset( const App::ECS::Entity p_preset )
		{
			if ( _comboBox->count() == 0 )
			{
				refresh();
			}

			const int index = _comboBox->findData( QVariant::fromValue<App::ECS::Entity>( p_preset ) );
			if ( index != -1 )
			{
				_comboBox->setCurrentIndex( index );
			}
		}

	  private:
		/**
		 * @brief Preset list.
		 */
		QPointer<QComboBox> _comboBox;

		/**
		 * @brief Line edit to rename the preset.
		 */
		QPointer<QLineEdit> _lineRename;

		/**
		 * @brief Refresh the combo box when presets are added or removed.
		 */
		void _refreshComboBox( App::ECS::Registry &, App::ECS::Entity )
		{
			using namespace App;
			QSignalBlocker blocker( _comboBox );

			const ECS::Entity currentPreset = getCurrentPreset();
			_comboBox->clear();

			int											 indexToSelect = -1;
			auto										 view		   = REG().view<Preset::Name, P>();
			std::vector<std::pair<QString, ECS::Entity>> presets;
			presets.reserve( view.size_hint() );
			for ( const ECS::Entity entity : view )
			{
				const auto & presetName = view.template get<Preset::Name>( entity ).name;
				presets.emplace_back( QString::fromStdString( presetName ), entity );
			}

			std::sort(
				presets.begin(),
				presets.end(),
				[]( const auto & p_left, const auto & p_right )
				{ return QString::localeAwareCompare( p_left.first, p_right.first ) < 0; }
			);

			for ( int i = 0; i < int( presets.size() ); ++i )
			{
				const auto & [ presetName, entity ] = presets[ i ];
				_comboBox->addItem( presetName, QVariant::fromValue<ECS::Entity>( entity ) );

				if ( entity == currentPreset )
				{
					indexToSelect = i;
				}
			}

			if ( _comboBox->count() > 0 )
			{
				indexToSelect = std::clamp( indexToSelect, 0, _comboBox->count() - 1 );
				_comboBox->setCurrentIndex( indexToSelect );
			}

			_lineRename->setText( _comboBox->currentText() );
			emit presetChanged( getCurrentPreset() );
		}

		/**
		 * @brief Update preset name in the combo box.
		 */
		void _onPresetNameUpdated( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			if ( not p_r.all_of<P>( p_e ) )
			{
				return;
			}

			const QSignalBlocker blocker( _comboBox );
			const QString		 newName = QString::fromStdString( p_r.get<App::Preset::Name>( p_e ).name );
			const int			 index	 = _comboBox->findData( QVariant::fromValue<App::ECS::Entity>( p_e ) );
			if ( index != -1 )
			{
				_comboBox->setItemText( index, newName );
				if ( _comboBox->currentIndex() == index )
				{
					_lineRename->setText( newName );
				}
			}
		}

		/**
		 * @brief Trigger signal when the current preset is updated from App.
		 */
		void _onUpdatePreset( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			if ( p_e == getCurrentPreset() )
			{
				emit currentPresetUpdated( p_e );
			}
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
