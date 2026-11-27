#ifndef VTX_UI_QT_WIDGET_LIBRARY_PRESET_SELECTOR_HPP
#define VTX_UI_QT_WIDGET_LIBRARY_PRESET_SELECTOR_HPP

#include "read_only_notice.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSignalBlocker>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>
#include <Qt>
#include <algorithm>
#include <app/action/preset.hpp>
#include <app/ecs.hpp>
#include <app/generic/name.hpp>
#include <app/generic/readonly.hpp>
#include <app/services.hpp>
#include <optional>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/math.hpp>
#include <utility>
#include <vector>

namespace VTX::UI::QT::Widget::Library
{
	/**
	 * @brief Subset of presets displayed by a preset selector.
	 */
	enum struct E_PRESET_SCOPE : uint8_t
	{
		ALL,
		BUILT_IN,
		USER
	};

	/**
	 * @brief Item data role holding the scope of a preset folder.
	 */
	inline constexpr int PRESET_SCOPE_ROLE = Qt::UserRole + 1;

	/**
	 * @brief Check if a preset is in the given scope.
	 */
	inline bool isInScope( const Entity p_preset, const E_PRESET_SCOPE p_scope )
	{
		switch ( p_scope )
		{
		case E_PRESET_SCOPE::BUILT_IN: return App::REG().all_of<App::Generic::ReadOnly>( p_preset );
		case E_PRESET_SCOPE::USER: return not App::REG().all_of<App::Generic::ReadOnly>( p_preset );
		case E_PRESET_SCOPE::ALL: break;
		}
		return true;
	}

	/**
	 * @brief Base class for preset selectors to permit signals (not possible on templated class).
	 */
	class BasePresetSelector : public QGroupBox
	{
		Q_OBJECT

	  public:
		BasePresetSelector( QWidget * const p_parent ) : QGroupBox( p_parent ) {}

		~BasePresetSelector() override = default;

	  signals:
		/**
		 * @brief Signal emitted when the selected preset is changed from the widget.
		 */
		void presetChanged( Entity );

		/**
		 * @brief Signal emitted when the current preset is updated from App.
		 */
		void currentPresetUpdated( Entity );

		/**
		 * @brief Signal emitted when the user selects a preset.
		 */
		void presetSelected( Entity );
	};

	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<typename P>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * const p_parent ) :
			BasePresetSelector( p_parent ), _comboBox( Helper::makeQObject<QComboBox>( this ) ),
			_lineRename( Helper::makeQObject<QLineEdit>( this ) )
		{
			auto * layout = Helper::makeQObject<QVBoxLayout>( this );
			setTitle( "Presets" );

			// layout->setContentsMargins( 0, 0, 0, 0 );

			layout->addWidget( _comboBox );

			auto * toolbar = Helper::makeQObject<QToolBar>( this );
			toolbar->setMovable( false );
			toolbar->setFloatable( false );
			toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );
			// toolbar->setIconSize( QSize( 12, 12 ) );
			auto * bNew = _addButton( *toolbar, "New", Style::Icons::NEW, "Create a new empty preset" );
			auto * bDuplicate
				= _addButton( *toolbar, "Duplicate", Style::Icons::COPY, "Create a new preset from this one" );

			layout->addWidget( toolbar );

			layout->addWidget( _lineRename );

			auto * bApply = Helper::makeQObject<QPushButton>( "Apply", this );
			bApply->setIcon( STYLE().iconFromCodepoint( Style::Icons::APPLY ) );
			bApply->setToolTip( "Apply this preset" );
			bApply->setStatusTip( "Apply this preset to the whole scene" );

			layout->addWidget( bApply );

			connect(
				_comboBox,
				&QComboBox::currentIndexChanged,
				this,
				[ this ]( const int )
				{
					_updateRenameLine();
					emit presetChanged( getCurrentPreset() );
				}
			);

			connect(
				_comboBox,
				&QComboBox::activated,
				this,
				[ this ]( const int ) { emit presetSelected( getCurrentPreset() ); }
			);

			connect(
				bNew,
				&QToolButton::clicked,
				this,
				[ this ]()
				{
					_lastConstructedPreset = InvalidEntity;
					App::ACTION().execute<App::Action::Preset::Add<P>>( std::nullopt, std::nullopt );
					_selectLastConstructedPreset();
				}
			);

			connect( bDuplicate, &QToolButton::clicked, this, [ this ]() { duplicateCurrentPreset(); } );

			connect(
				bApply,
				&QToolButton::clicked,
				this,
				[ this ]()
				{
					const Entity preset = getCurrentPreset();
					if ( preset != InvalidEntity )
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
					const Entity preset = getCurrentPreset();
					if ( preset == InvalidEntity )
					{
						return;
					}
					App::ACTION().execute<App::Action::Preset::Rename<P>>( preset, _lineRename->text().toStdString() );
				}
			);

			// Callbacks.
			auto & reg = App::REG();

			_onConstructConnection
				= reg.on_construct<P>().template connect<&PresetSelector::_onPresetConstructed>( this );
			_onDestroyConnection = reg.on_destroy<P>().template connect<&PresetSelector::_refreshComboBox>( this );
			_onNameUpdateConnection
				= reg.on_update<App::Generic::Name>().template connect<&PresetSelector::_onPresetNameUpdated>( this );
			_onPresetUpdateConnection = reg.on_update<P>().template connect<&PresetSelector::_onUpdatePreset>( this );
		}

		Entity getCurrentPreset() const
		{
			const QVariant data = _comboBox->currentData();
			return data.isValid() ? data.value<Entity>() : InvalidEntity;
		}

		void setScope( const E_PRESET_SCOPE p_scope )
		{
			if ( _scope == p_scope )
			{
				return;
			}

			_scope = p_scope;
			refresh();
		}

		void duplicateCurrentPreset()
		{
			const Entity preset = getCurrentPreset();
			if ( preset == InvalidEntity )
			{
				return;
			}

			_lastConstructedPreset = InvalidEntity;
			App::ACTION().execute<App::Action::Preset::Duplicate<P>>( preset, std::nullopt );
			_selectLastConstructedPreset();
		}

		void refresh() { _refreshComboBox( App::REG(), Entity {} ); }

		void setCurrentPreset( const Entity p_preset )
		{
			if ( _comboBox->count() == 0 )
			{
				refresh();
			}

			const int index = _comboBox->findData( QVariant::fromValue<Entity>( p_preset ) );
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
		 * @brief Last preset constructed in App, used to select a preset created from this widget.
		 */
		Entity _lastConstructedPreset = InvalidEntity;

		/**
		 * @brief Subset of presets displayed in the combo box.
		 */
		E_PRESET_SCOPE _scope = E_PRESET_SCOPE::ALL;

		/**
		 * @brief Connections to App.
		 */
		VTX::Util::EventHub::ScopedConnection _onConstructConnection;
		VTX::Util::EventHub::ScopedConnection _onDestroyConnection;
		VTX::Util::EventHub::ScopedConnection _onNameUpdateConnection;
		VTX::Util::EventHub::ScopedConnection _onPresetUpdateConnection;

		QToolButton * _addButton(
			QToolBar &			   p_toolbar,
			const QString &		   p_text,
			const Style::Codepoint p_icon,
			const QString &		   p_tip
		)
		{
			auto * const button = Helper::makeQObject<QToolButton>( &p_toolbar );
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

		void _onPresetConstructed( Registry & p_r, Entity p_e )
		{
			_lastConstructedPreset = p_e;
			_refreshComboBox( p_r, p_e );
		}

		void _selectLastConstructedPreset()
		{
			if ( App::REG().valid( _lastConstructedPreset ) )
			{
				App::ACTION().execute<App::Action::Preset::Apply<P>>( _lastConstructedPreset );
				setCurrentPreset( _lastConstructedPreset );
				emit presetSelected( _lastConstructedPreset );
			}
		}

		/**
		 * @brief Refresh the combo box when presets are added or removed.
		 */
		void _refreshComboBox( Registry &, Entity )
		{
			using namespace App;
			const QSignalBlocker blocker( _comboBox );

			const Entity currentPreset = getCurrentPreset();
			_comboBox->clear();

			int										indexToSelect = -1;
			auto									view		  = REG().view<Generic::Name, P>();
			std::vector<std::pair<QString, Entity>> presets;
			presets.reserve( view.size_hint() );
			for ( const Entity entity : view )
			{
				if ( not isInScope( entity, _scope ) )
				{
					continue;
				}

				const auto & presetName = view.template get<Generic::Name>( entity ).name;
				presets.emplace_back( QString::fromStdString( presetName ), entity );
			}

			std::sort(
				presets.begin(),
				presets.end(),
				[]( const auto & p_left, const auto & p_right )
				{ return QString::localeAwareCompare( p_left.first, p_right.first ) < 0; }
			);

			for ( int i = 0; std::cmp_less( i, presets.size() ); ++i )
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
				indexToSelect = VTX::Util::Math::clamp( indexToSelect, 0, _comboBox->count() - 1 );
				_comboBox->setCurrentIndex( indexToSelect );
			}

			_updateRenameLine();
			emit presetChanged( getCurrentPreset() );
		}

		void _updateRenameLine()
		{
			const Entity preset = getCurrentPreset();
			_lineRename->setText( _comboBox->currentText() );
			_lineRename->setVisible( not ReadOnlyNotice::isReadOnly( preset ) );
		}

		/**
		 * @brief Update preset name in the combo box.
		 */
		void _onPresetNameUpdated( Registry & p_r, Entity p_e )
		{
			if ( not p_r.all_of<P>( p_e ) )
			{
				return;
			}

			const QSignalBlocker blocker( _comboBox );
			const QString		 newName = QString::fromStdString( p_r.get<App::Generic::Name>( p_e ).name );
			const int			 index	 = _comboBox->findData( QVariant::fromValue<Entity>( p_e ) );
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
		void _onUpdatePreset( Registry & p_r, Entity p_e )
		{
			if ( p_e == getCurrentPreset() )
			{
				emit currentPresetUpdated( p_e );
			}
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
