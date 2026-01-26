#include "ui/qt/dock_widget/scene.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/system/trajectory.hpp"
#include "ui/qt/delegate/scene_item_delegate.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QToolBar>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( p_parent, "Scene" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		auto * layout = new QVBoxLayout( this );
		layout->setContentsMargins( 0, 0, 0, 0 );
		auto * toolbar = new QToolBar( this );
		// toolbar->setIconSize( QSize( 18, 18 ) );
		auto * actionExpandAll	 = toolbar->addAction( tr( "Expand" ) );
		auto * actionCollapseAll = toolbar->addAction( tr( "Reduce" ) );
		_layout->addWidget( toolbar );

		_tree = new Widget::Tree( this );
		_tree->setModel( &MODEL() );
		_tree->setSelectionModel( &SELECTION() );
		_tree->setItemDelegate( new Delegate::SceneItemDelegate( _tree ) );
		_layout->addWidget( _tree );

		connect( actionExpandAll, &QAction::triggered, this, [ this ] { _tree->expandAll(); } );
		connect( actionCollapseAll, &QAction::triggered, this, [ this ] { _tree->collapseAll(); } );
	}

} // namespace VTX::UI::QT::DockWidget
