#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <app/action/preset.hpp>
#include <app/ecs.hpp>
#include <app/preset/name.hpp>
#include <app/services.hpp>

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
			toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			toolbar->setIconSize( QSize( 18, 18 ) );
			auto * aNew		  = Application::getAction<Preset::Add<P>>();
			auto * aDuplicate = Application::getAction<Preset::Duplicate<P>>();
			auto * aDelete	  = Application::getAction<Preset::Delete<P>>();
			toolbar->addAction( aNew );
			toolbar->addAction( aDuplicate );
			toolbar->addAction( aDelete );
			layout->addWidget( toolbar );

			aNew->setIcon( QIcon::fromTheme( "Search" ) );
			aDuplicate->setIcon( QIcon::fromTheme( QIcon::ThemeIcon::DocumentNew ) );

			_lineRename = new QLineEdit( this );
			layout->addWidget( _lineRename );

			connect(
				_comboBox,
				&QComboBox::currentTextChanged,
				this,
				[ this ]()
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
				aDelete,
				&QAction::triggered,
				[ this ]() { App::ACTION().execute<App::Action::Preset::Delete<P>>( getCurrentPreset() ); }
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
			App::HUB().connect<App::Events::PresetRename, &PresetSelector::_onPresetRename>( this );
			reg.on_update<P>().template connect<&PresetSelector::_onUpdatePreset>( this );
			// reg.on_construct<App ::Preset::Instance<P>>().template connect<&PresetSelector::_onSelectPreset>( this );
		}

		inline App::ECS::Entity getCurrentPreset() const { return _comboBox->currentData().value<App::ECS::Entity>(); }

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
		 * @brief Select the preset in the combo box when a preset is set as current from App.
		 */
		/*
		void _onSelectPreset( const App::ECS::Entity p_e )
		{
			auto &			 preset = App::REG().get<App::Preset::Instance<P>>( p_e );
			App::ECS::Entity ent	= preset.entity;
			const int		 index	= _comboBox->findData( QVariant::fromValue<App::ECS::Entity>( ent ) );
			if ( index != -1 )
			{
				_comboBox->setCurrentIndex( index );
			}
			emit presetChanged( ent );
		}
		*/

		/**
		 * @brief Refresh the combo box when presets are added or removed.
		 */
		void _refreshComboBox( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			using namespace App;
			QSignalBlocker blocker( _comboBox );

			// Emit
			_comboBox->clear();

			// TODO: store old entity, dont select the new one.
			int	 indexToSelect = -1;
			int	 i			   = 0;
			auto view		   = REG().view<Preset::Name, P>();
			for ( const ECS::Entity entity : view )
			{
				const auto & presetName = view.template get<Preset::Name>( entity ).name;
				_comboBox->addItem( QString::fromStdString( presetName ), QVariant::fromValue<ECS::Entity>( entity ) );

				if ( entity == p_e )
				{
					indexToSelect = i;
				}

				++i;
			}

			indexToSelect = std::clamp( indexToSelect, 0, _comboBox->count() - 1 );
			_comboBox->setCurrentIndex( indexToSelect );
			_lineRename->setText( _comboBox->currentText() );
			emit presetChanged( p_e );
		}

		/**
		 * @brief Update widget when a preset is renamed from App.
		 */
		void _onPresetRename( const App::Events::PresetRename & p_event )
		{
			const QSignalBlocker blocker( _comboBox );
			const QString		 newName = QString::fromStdString( p_event.name );
			int					 index = _comboBox->findData( QVariant::fromValue<App::ECS::Entity>( p_event.preset ) );
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
