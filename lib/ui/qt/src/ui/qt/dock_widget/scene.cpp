#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/widget/tree/color_layout_presets.hpp"
#include "ui/qt/widget/tree/graphics_config_presets.hpp"
#include "ui/qt/widget/tree/representation_presets.hpp"
#include <app/action/application.hpp>
#include <app/ecs.hpp>
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <app/system/trajectory.hpp>
#include <app/system/visibility.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	namespace
	{
		void _setSelectionEnabled( QAbstractItemView * const p_view, const bool p_enabled )
		{
			if ( p_view == nullptr )
			{
				return;
			}

			p_view->setSelectionMode( p_enabled ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection );
		}
	}

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SCENE ) );

		_treeGraphicsConfigPresets = new Widget::Tree::GraphicsConfigPresets( this );
		_treeColorLayoutPresets	   = new Widget::Tree::ColorLayoutPresets( this );
		_treeRepresentationPresets = new Widget::Tree::RepresentationPresets( this );

		_layout->addWidget( _treeGraphicsConfigPresets );
		_layout->addWidget( _treeColorLayoutPresets );
		_layout->addWidget( _treeRepresentationPresets );

		auto & selection = SELECTION();
		selection.add( _treeGraphicsConfigPresets->selectionModel(), E_SELECTION_GROUP::GRAPHICS_CONFIG );
		selection.add( _treeColorLayoutPresets->selectionModel(), E_SELECTION_GROUP::COLOR_LAYOUT );
		selection.add( _treeRepresentationPresets->selectionModel(), E_SELECTION_GROUP::REPRESENTATION );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		App::REG().on_construct<Renderer::Camera>().connect<&Scene::_onCameraConstruct>( this );

		App::HUB().connect<App::Events::SystemLoad, &Scene::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::Topology>().connect<&Scene::_onSystemDestroy>( this );
		App::REG().on_update<App::System::Visibility>().connect<&Scene::_onUpdateVisibility>( this );
		App::REG().on_update<App::System::Selection>().connect<&Scene::_onUpdateSelection>( this );
		App::REG().on_construct<App::System::TrajectoryFullBuffer>().connect<&Scene::_onTrajectoryCreated>( this );

		App::HUB().connect<Events::SelectionLocked, &Scene::_onSelectionLocked>( this );

		App::HUB().connect<App::Events::ThreadProgress, &Scene::_onThreadProgress>( this );
		App::HUB().connect<App::Events::ThreadTerminated, &Scene::_onThreadTerminated>( this );

		_onSelectionLocked( Events::SelectionLocked { UI_ACTIONS().isChecked( Action::Selection::LOCK ) } );
	}

	void Scene::_onCameraConstruct( App::Registry &, App::Entity p_e )
	{
		_treeCamera = new Widget::Tree::Camera( p_e, this );
		SELECTION().add( _treeCamera->selectionModel(), E_SELECTION_GROUP::CAMERA );
		_layout->insertWidget( _layout->indexOf( _filler ), _treeCamera );
		_setSelectionEnabled( _treeCamera, not UI_ACTIONS().isChecked( Action::Selection::LOCK ) );
	}

	void Scene::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		const auto entity = p_e.system;
		auto *	   tree	  = new Widget::Tree::System( entity, this );

		tree->setSelectionMode( QAbstractItemView::NoSelection );

		assert( not _mapSystemTreeWidgets.contains( entity ) );
		_mapSystemTreeWidgets.emplace( entity, tree );
		_layout->insertWidget( _layout->indexOf( _filler ), tree );

		if ( App::System::hasMultiFrameTrajectory( p_e.system ) )
			_onTrajectoryCreated( App::REG(), p_e.system );
	}

	void Scene::_onSystemDestroy( App::Registry &, App::Entity p_e )
	{
		assert( _mapSystemTreeWidgets.contains( p_e ) );
		_mapSystemTreeWidgets[ p_e ]->deleteLater();
		_mapSystemTreeWidgets.erase( p_e );

		if ( _mapTrajTreeWidgets.contains( p_e ) )
		{
			_mapTrajTreeWidgets[ p_e ]->deleteLater();
			_mapTrajTreeWidgets.erase( p_e );
		}
	}

	void Scene::_onUpdateVisibility( App::Registry &, App::Entity p_e )
	{
		assert( _mapSystemTreeWidgets.contains( p_e ) );
		_mapSystemTreeWidgets[ p_e ]->viewport()->update();
	}

	void Scene::_onUpdateSelection( App::Registry & p_r, App::Entity p_e )
	{
		// Do not clear if full deselection.
		if ( p_r.get<App::System::Selection>( p_e ).atoms.any() )
		{
			SELECTION().clear();
		}

		assert( _mapSystemTreeWidgets.contains( p_e ) );
		_mapSystemTreeWidgets[ p_e ]->viewport()->update();
	}

	void Scene::_onSelectionLocked( const Events::SelectionLocked & p_event )
	{
		const bool selectionEnabled = not p_event.locked;
		_setSelectionEnabled( _treeGraphicsConfigPresets, selectionEnabled );
		_setSelectionEnabled( _treeColorLayoutPresets, selectionEnabled );
		_setSelectionEnabled( _treeRepresentationPresets, selectionEnabled );
		_setSelectionEnabled( _treeCamera, selectionEnabled );
	}
	void Scene::_onTrajectoryCreated( App::Registry &, App::Entity p_entity )
	{
		if ( not _mapSystemTreeWidgets.contains( p_entity ) )
			return;

		if ( _mapTrajTreeWidgets.contains( p_entity ) )
			_mapTrajTreeWidgets.erase( p_entity );
		auto * player = new Widget::Tree::TrajectoryPlayer( p_entity, this );
		_mapTrajTreeWidgets.emplace( p_entity, player );
		_layout->insertWidget( _layout->indexOf( _mapSystemTreeWidgets[ p_entity ] ), player );
	}

	void Scene::_onThreadProgress( const App::Events::ThreadProgress & p_event )
	{
		if ( not _mapThreadWidgets.contains( p_event.id ) )
		{
			_mapThreadWidgets.emplace( p_event.id, new Widget::Thread( this ) );
			_layout->insertWidget( _layout->indexOf( _filler ), _mapThreadWidgets[ p_event.id ] );

			connect(
				_mapThreadWidgets[ p_event.id ],
				&Widget::Thread::cancelClicked,
				this,
				[ p_event ]() { App::ACTION().execute<App::Action::Application::StopThread>( p_event.id ); }
			);
		}

		_mapThreadWidgets[ p_event.id ]->set( QString::fromStdString( p_event.text ), p_event.progress );
	}

	void Scene::_onThreadTerminated( const App::Events::ThreadTerminated & p_event )
	{
		assert( _mapThreadWidgets.contains( p_event.id ) );

		_mapThreadWidgets[ p_event.id ]->deleteLater();
		_mapThreadWidgets.erase( p_event.id );
	}

} // namespace VTX::UI::QT::DockWidget
