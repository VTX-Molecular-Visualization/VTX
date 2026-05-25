#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QSignalBlocker>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>
#include <algorithm>
#include <app/action/preset.hpp>
#include <app/ecs.hpp>
#include <app/preset/name.hpp>
#include <app/services.hpp>
#include <optional>
#include <utility>
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
		void presetChanged( const App::Entity );

		/**
		 * @brief Signal emitted when the current preset is updated from App.
		 */
		void currentPresetUpdated( const App::Entity );
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

			auto * toolbar = new QToolBar( this );
			toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );
			// toolbar->setIconSize( QSize( 12, 12 ) );
			auto * bNew		  = _addButton( *toolbar, "New", Style::Icons::NEW, "Create a new empty preset" );
			auto * bDuplicate
				= _addButton( *toolbar, "Duplicate", Style::Icons::COPY, "Create a new preset from this one" );
			auto * bApply	  = _addButton( *toolbar, "Apply", Style::Icons::APPLY, "Apply this preset" );
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
				bNew,
				&QToolButton::clicked,
				this,
				[]() { App::ACTION().execute<App::Action::Preset::Add<P>>( std::nullopt, std::nullopt ); }
			);

			connect(
				bDuplicate,
				&QToolButton::clicked,
				this,
				[ this ]()
				{
					const App::Entity preset = getCurrentPreset();
					if ( preset != App::ECS::InvalidEntity )
					{
						App::ACTION().execute<App::Action::Preset::Duplicate<P>>( preset, std::nullopt );
					}
				}
			);

			connect(
				bApply,
				&QToolButton::clicked,
				this,
				[ this ]()
				{
					const App::Entity preset = getCurrentPreset();
					if ( preset != App::ECS::InvalidEntity )
					{
						App::ACTION().execute<App::Action::Preset::Apply<P>>( preset );
					}
				}
			);

			connect(
				_lineRename,
				&QLineEdit::editingFinished,
				this,
				[ this ]()
				{
					App::ACTION().execute<App::Action::Preset::Rename<P>>(
						getCurrentPreset(), _lineRename->text().toStdString()
					);
				}
			);

			// Callbacks.
			auto & reg = App::REG();

			_onConstructConnection = reg.on_construct<P>().template connect<&PresetSelector::_refreshComboBox>( this );
			_onDestroyConnection   = reg.on_destroy<P>().template connect<&PresetSelector::_refreshComboBox>( this );
			_onNameUpdateConnection
				= reg.on_update<App::Preset::Name>().template connect<&PresetSelector::_onPresetNameUpdated>( this );
			_onPresetUpdateConnection = reg.on_update<P>().template connect<&PresetSelector::_onUpdatePreset>( this );
		}

		virtual ~PresetSelector()
		{
			_onConstructConnection.release();
			_onDestroyConnection.release();
			_onNameUpdateConnection.release();
			_onPresetUpdateConnection.release();
		}

		inline App::Entity getCurrentPreset() const { return _comboBox->currentData().value<App::Entity>(); }
		inline void				refresh() { _refreshComboBox( App::REG(), App::Entity {} ); }
		inline void				setCurrentPreset( const App::Entity p_preset )
		{
			if ( _comboBox->count() == 0 )
			{
				refresh();
			}

			const int index = _comboBox->findData( QVariant::fromValue<App::Entity>( p_preset ) );
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
		 * @brief Connections to App.
		 */
		VTX::Util::EventHub::Connection _onConstructConnection;
		VTX::Util::EventHub::Connection _onDestroyConnection;
		VTX::Util::EventHub::Connection _onNameUpdateConnection;
		VTX::Util::EventHub::Connection _onPresetUpdateConnection;

		QToolButton * _addButton(
			QToolBar & p_toolbar, const QString & p_text, const Style::Codepoint p_icon, const QString & p_tip
		)
		{
			auto * const button = new QToolButton( &p_toolbar );
			button->setText( p_text );
			button->setIcon( STYLE().iconFromCodepoint( p_icon ) );
			button->setToolTip( p_tip );
			button->setWhatsThis( p_tip );
			button->setStatusTip( p_tip );
			button->setToolButtonStyle( p_toolbar.toolButtonStyle() );
			button->setAutoRaise( true );
			p_toolbar.addWidget( button );
			return button;
		}

		/**
		 * @brief Refresh the combo box when presets are added or removed.
		 */
		void _refreshComboBox( App::Registry &, App::Entity )
		{
			using namespace App;
			QSignalBlocker blocker( _comboBox );

			const Entity currentPreset = getCurrentPreset();
			_comboBox->clear();

			int											 indexToSelect = -1;
			auto										 view		   = REG().view<Preset::Name, P>();
			std::vector<std::pair<QString, Entity>> presets;
			presets.reserve( view.size_hint() );
			for ( const Entity entity : view )
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
				_comboBox->addItem( presetName, QVariant::fromValue<Entity>( entity ) );

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
		void _onPresetNameUpdated( App::Registry & p_r, App::Entity p_e )
		{
			if ( not p_r.all_of<P>( p_e ) )
			{
				return;
			}

			const QSignalBlocker blocker( _comboBox );
			const QString		 newName = QString::fromStdString( p_r.get<App::Preset::Name>( p_e ).name );
			const int			 index	 = _comboBox->findData( QVariant::fromValue<App::Entity>( p_e ) );
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
		void _onUpdatePreset( App::Registry & p_r, App::Entity p_e )
		{
			if ( p_e == getCurrentPreset() )
			{
				emit currentPresetUpdated( p_e );
			}
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
