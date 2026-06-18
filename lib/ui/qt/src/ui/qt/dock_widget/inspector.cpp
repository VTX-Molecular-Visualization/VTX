#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/camera.hpp"
#include "ui/qt/widget/controller.hpp"
#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/widget/selection.hpp"
#include <QFontDatabase>
#include <QLabel>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * p_parent ) : BaseDockWidget( p_parent, "Selection" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SELECTION ) );

		_lockButton = new QToolButton( this );
		_lockButton->setDefaultAction( UI_ACTIONS().getAction( Action::Selection::LOCK ) );
		_lockButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		_lockButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		_layout->addWidget( _lockButton );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		connect(
			&SELECTION(),
			&SelectionManager::selectionChanged,
			this,
			[ this ]( const E_SELECTION_GROUP p_group, const QItemSelection & p_selection )
			{
				if ( p_selection.isEmpty() )
				{
					_clear();
					return;
				}

				const QModelIndex index
					= p_selection.indexes().isEmpty() ? QModelIndex() : p_selection.indexes().first();
				const QVariant data		 = index.data( Qt::UserRole );
				const Entity   preset	 = data.value<Entity>();
				const bool	   hasPreset = data.isValid();

				// Insert widget.
				switch ( p_group )
				{
				case E_SELECTION_GROUP::CAMERA:
				{
					_clear();
					if ( hasPreset )
					{
						_viewPointWidget = new Widget::ViewPoint( preset, this );
						_layout->insertWidget( _layout->indexOf( _filler ), _viewPointWidget );
					}
					else
					{
						const auto [ ent, _ ]	= App::ECS::getFirstEntityWithComponents<Renderer::Camera>();
						auto * cameraWidget		= new Widget::Camera( ent, this );
						auto * controllerWidget = new Widget::Controller( ent, this );
						_layout->insertWidget( _layout->indexOf( _filler ), cameraWidget );
						_layout->insertWidget( _layout->indexOf( _filler ), controllerWidget );
					}
					break;
				}
				case E_SELECTION_GROUP::GRAPHICS_CONFIG:
				{
					if ( _graphicsConfigWidget )
					{
						if ( hasPreset )
						{
							_graphicsConfigWidget->setCurrentPreset( preset );
						}
						else
						{
							_graphicsConfigWidget->refresh();
						}
						break;
					}

					_clear();
					_graphicsConfigWidget = new Widget::Library::GraphicsConfig( this );
					if ( hasPreset )
					{
						_graphicsConfigWidget->setCurrentPreset( preset );
					}
					else
					{
						_graphicsConfigWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _graphicsConfigWidget );
					break;
				}
				case E_SELECTION_GROUP::COLOR_LAYOUT:
				{
					if ( _colorLayoutWidget )
					{
						if ( hasPreset )
						{
							_colorLayoutWidget->setCurrentPreset( preset );
						}
						else
						{
							_colorLayoutWidget->refresh();
						}
						break;
					}

					_clear();
					_colorLayoutWidget = new Widget::Library::ColorLayout( this );
					if ( hasPreset )
					{
						_colorLayoutWidget->setCurrentPreset( preset );
					}
					else
					{
						_colorLayoutWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _colorLayoutWidget );
					break;
				}
				case E_SELECTION_GROUP::REPRESENTATION:
				{
					if ( _representationWidget )
					{
						if ( hasPreset )
						{
							_representationWidget->setCurrentPreset( preset );
						}
						else
						{
							_representationWidget->refresh();
						}
						break;
					}

					_clear();
					_representationWidget = new Widget::Library::Representation( this );
					if ( hasPreset )
					{
						_representationWidget->setCurrentPreset( preset );
					}
					else
					{
						_representationWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _representationWidget );
					break;
				}
				default: break;
				}
			}
		);

		App::REG().on_update<App::System::Selection>().connect<&Inspector::_onSystemSelectionUpdated>( this );
		App::REG().on_destroy<Core::Struct::Topology>().connect<&Inspector::_onSystemSelectionUpdated>( this );
	}

	void Inspector::_clear()
	{
		for ( int i = _layout->count() - 1; i >= 0; --i )
		{
			if ( _layout->itemAt( i )->widget() != _filler && _layout->itemAt( i )->widget() != _lockButton )
			{
				QWidget * w = _layout->itemAt( i )->widget();
				if ( w == _selectionListWidget )
				{
					_selectionListWidget = nullptr;
				}
				if ( w == _graphicsConfigWidget )
				{
					_graphicsConfigWidget = nullptr;
				}
				if ( w == _colorLayoutWidget )
				{
					_colorLayoutWidget = nullptr;
				}
				if ( w == _representationWidget )
				{
					_representationWidget = nullptr;
				}
				if ( w == _viewPointWidget )
				{
					_viewPointWidget = nullptr;
				}
				_layout->removeWidget( w );
				w->deleteLater();
			}
		}
	}

	bool Inspector::_hasSystemSelection() const
	{
		for ( const Entity entity : App::REG().view<App::System::Selection>() )
		{
			if ( App::REG().get<App::System::Selection>( entity ).atoms.any() )
			{
				return true;
			}
		}

		return false;
	}

	void Inspector::_onSystemSelectionUpdated( Registry &, Entity )
	{
		Util::ScopedChrono timer( "Inspector::_onSystemSelectionUpdated" );

		if ( not _hasSystemSelection() )
		{
			if ( _selectionListWidget )
			{
				_clear();
			}
			return;
		}

		// Already displaying system selection.
		// if ( _selectionListWidget )
		//{
		//	_selectionListWidget->refresh();
		//	return;
		//}

		_clear();
		_selectionListWidget = new Widget::Selection( this );
		_layout->insertWidget( _layout->indexOf( _filler ), _selectionListWidget );
	}

} // namespace VTX::UI::QT::DockWidget
