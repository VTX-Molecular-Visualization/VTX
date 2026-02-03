#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/services.hpp"
#include <app/ecs.hpp >
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		_treeCamera = new Widget::Tree::Camera( this );
		_layout->addWidget( _treeCamera );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		App::HUB().connect<App::Events::SystemLoad, &Scene::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Scene::_onSystemDestroy>( this );
		App::REG().on_update<App::System::Selection>().connect<&Scene::_onUpdateSelection>( this );

		App::HUB().connect<Events::SelectionLocked, &Scene::_onSelectionLocked>( this );
	}

	void Scene::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		const auto entity = p_e.system;
		auto *	   tree	  = new Widget::Tree::System( entity, this );

		const bool selectionLocked = SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool();
		tree->setSelectionMode(
			selectionLocked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
		);

		assert( not _mapTreeWidgets.contains( entity ) );
		_mapTreeWidgets.emplace( entity, tree );
		_layout->insertWidget( _layout->indexOf( _filler ), tree );
	}

	void Scene::_onSystemDestroy( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		assert( _mapTreeWidgets.contains( p_e ) );
		_mapTreeWidgets[ p_e ]->deleteLater();
		_mapTreeWidgets.erase( p_e );
	}

	void Scene::_onUpdateSelection( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		assert( _mapTreeWidgets.contains( p_e ) );
		_mapTreeWidgets[ p_e ]->getSystemSelectionModel().refresh();
	}

	void Scene::_onSelectionLocked( const Events::SelectionLocked & p_event )
	{
		for ( const auto & [ _, w ] : _mapTreeWidgets )
		{
			w->setSelectionMode(
				p_event.locked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
			);
		}
	}

} // namespace VTX::UI::QT::DockWidget
