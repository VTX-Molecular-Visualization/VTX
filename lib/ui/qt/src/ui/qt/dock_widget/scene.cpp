#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/widget/tree/color_layout_presets.hpp"
#include "ui/qt/widget/tree/graphics_config_presets.hpp"
#include "ui/qt/widget/tree/representation_presets.hpp"
#include <app/ecs.hpp >
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <app/system/visibility.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SCHEMA ) );

		_layout->addWidget( new Widget::Tree::GraphicsConfigPresets( this ) );
		_layout->addWidget( new Widget::Tree::ColorLayoutPresets( this ) );
		_layout->addWidget( new Widget::Tree::RepresentationPresets( this ) );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		App::REG().on_construct<Renderer::Camera>().connect<&Scene::_onCameraConstruct>( this );

		App::HUB().connect<App::Events::SystemLoad, &Scene::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Scene::_onSystemDestroy>( this );
		App::REG().on_update<App::System::Visibility>().connect<&Scene::_onUpdateVisibility>( this );
		App::REG().on_update<App::System::Selection>().connect<&Scene::_onUpdateSelection>( this );

		App::HUB().connect<Events::SelectionLocked, &Scene::_onSelectionLocked>( this );
	}

	void Scene::_onCameraConstruct( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		_layout->insertWidget( _layout->indexOf( _filler ), new Widget::Tree::Camera( p_e, this ) );
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

	void Scene::_onUpdateVisibility( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		assert( _mapTreeWidgets.contains( p_e ) );
		_mapTreeWidgets[ p_e ]->viewport()->update();
	}

	void Scene::_onUpdateSelection( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		assert( _mapTreeWidgets.contains( p_e ) );
		_mapTreeWidgets[ p_e ]->getSystemSelectionModel().refresh();
		_mapTreeWidgets[ p_e ]->viewport()->update();
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
