#include "ui/qt/dock_widget/scene.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/system/trajectory.hpp"
#include "ui/qt/delegate/scene_item_delegate.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QToolBar>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// auto * layout = new QVBoxLayout( this );
		// layout->setContentsMargins( 0, 0, 0, 0 );
		// layout->setSpacing( 0 );

		_treeCamera = new Widget::Tree::Camera( this );

		_tree = new Widget::Tree::System( this );
		_tree->setModel( &MODEL() );
		_tree->setSelectionModel( &SELECTION() );
		_tree->setItemDelegate( new Delegate::SceneItemDelegate( _tree ) );

		_layout->addWidget( _treeCamera );
		_layout->addWidget( _tree );
		_layout->addStretch( 1 );

		const bool selectionLocked = SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool();
		_tree->setSelectionMode(
			selectionLocked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
		);

		App::HUB().connect<Events::SelectionLocked, &Scene::_onSelectionLocked>( this );
	}

	void Scene::_onSelectionLocked( const Events::SelectionLocked & p_event )
	{
		_tree->setSelectionMode(
			p_event.locked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
		);
	}

} // namespace VTX::UI::QT::DockWidget
