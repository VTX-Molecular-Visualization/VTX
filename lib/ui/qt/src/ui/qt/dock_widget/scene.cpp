#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/widget/tree/color_layout_presets.hpp"
#include "ui/qt/widget/tree/graphics_config_presets.hpp"
#include "ui/qt/widget/tree/representation_presets.hpp"
#include <app/action/application.hpp>
#include <app/ecs.hpp >
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <app/system/trajectory.hpp>
#include <app/system/visibility.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SCENE ) );

		auto * treeGraphicsConfigPresets = new Widget::Tree::GraphicsConfigPresets( this );
		auto * treeColorLayoutPresets	 = new Widget::Tree::ColorLayoutPresets( this );
		auto * treeRepresentationPresets = new Widget::Tree::RepresentationPresets( this );

		_layout->addWidget( treeGraphicsConfigPresets );
		_layout->addWidget( treeColorLayoutPresets );
		_layout->addWidget( treeRepresentationPresets );

		auto & selection = SELECTION();
		selection.add( treeGraphicsConfigPresets->selectionModel(), E_SELECTION_GROUP::GRAPHICS_CONFIG );
		selection.add( treeColorLayoutPresets->selectionModel(), E_SELECTION_GROUP::COLOR_LAYOUT );
		selection.add( treeRepresentationPresets->selectionModel(), E_SELECTION_GROUP::REPRESENTATION );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		App::REG().on_construct<Renderer::Camera>().connect<&Scene::_onCameraConstruct>( this );

		App::HUB().connect<App::Events::SystemLoad, &Scene::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Scene::_onSystemDestroy>( this );
		App::REG().on_update<App::System::Visibility>().connect<&Scene::_onUpdateVisibility>( this );
		App::REG().on_update<App::System::Selection>().connect<&Scene::_onUpdateSelection>( this );
		App::REG().on_construct<App::System::TrajectoryFullBuffer>().connect<&Scene::_onTrajectoryCreated>( this );

		App::HUB().connect<Events::SelectionLocked, &Scene::_onSelectionLocked>( this );

		App::HUB().connect<App::Events::ThreadProgress, &Scene::_onThreadProgress>( this );
		App::HUB().connect<App::Events::ThreadTerminated, &Scene::_onThreadTerminated>( this );
	}

	void Scene::_onCameraConstruct( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		auto * treeCamera = new Widget::Tree::Camera( p_e, this );
		SELECTION().add( treeCamera->selectionModel(), E_SELECTION_GROUP::CAMERA );
		_layout->insertWidget( _layout->indexOf( _filler ), treeCamera );
	}

	void Scene::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		const auto entity = p_e.system;
		auto *	   tree	  = new Widget::Tree::System( entity, this );

		const bool selectionLocked = SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool();
		tree->setSelectionMode(
			selectionLocked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
		);
		SELECTION().add( tree->selectionModel(), E_SELECTION_GROUP::SYSTEM );

		assert( not _mapSystemTreeWidgets.contains( entity ) );
		_mapSystemTreeWidgets.emplace( entity, tree );
		_layout->insertWidget( _layout->indexOf( _filler ), tree );
	}

	void Scene::_onSystemDestroy( App::ECS::Registry &, App::ECS::Entity p_e )
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

	void Scene::_onUpdateVisibility( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		assert( _mapSystemTreeWidgets.contains( p_e ) );
		_mapSystemTreeWidgets[ p_e ]->viewport()->update();
	}

	void Scene::_onUpdateSelection( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		// Do not clear if full deselection.
		if ( p_r.get<App::System::Selection>( p_e ).atoms.any() )
		{
			SELECTION().clearBut( E_SELECTION_GROUP::SYSTEM );
		}

		assert( _mapSystemTreeWidgets.contains( p_e ) );
		_mapSystemTreeWidgets[ p_e ]->getSystemSelectionModel().refresh();
		_mapSystemTreeWidgets[ p_e ]->viewport()->update();
	}

	void Scene::_onSelectionLocked( const Events::SelectionLocked & p_event )
	{
		for ( const auto & [ _, w ] : _mapSystemTreeWidgets )
		{
			w->setSelectionMode(
				p_event.locked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
			);
		}
	}
	void Scene::_onTrajectoryCreated( App::ECS::Registry &, App::ECS::Entity p_entity )
	{
		assert( _mapSystemTreeWidgets.contains( p_entity ) );

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
