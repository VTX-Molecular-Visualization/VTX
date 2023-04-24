#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_viewpoint.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/view/ui/widget/path_scene_view.hpp"
#include "ui/qt/action/viewpoint.hpp"
#include <app/action/viewpoint.hpp>

#include <app/model/path.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuViewpoint::ContextualMenuViewpoint( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuViewpoint ::~ContextualMenuViewpoint() {}

	void ContextualMenuViewpoint::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuViewpoint::_setupSlots()
	{
		addAction( "Go to", this, &ContextualMenuViewpoint::_gotoAction );
		addSeparator();
		addAction( "Rename", this, &ContextualMenuViewpoint::_renameAction );
		addAction( "Relocate", this, &ContextualMenuViewpoint::_relocateAction );
		addAction( "Delete", this, &ContextualMenuViewpoint::_deleteAction );
	}

	void ContextualMenuViewpoint::localize() {}

	void ContextualMenuViewpoint::setTarget( Model::Viewpoint * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getDefaultName() ) );
	}

	void ContextualMenuViewpoint::_renameAction()
	{
		View::UI::Widget::PathSceneView * const pathSceneView
			= VTX::Core::MVC::MvcManager::get().getView<View::UI::Widget::PathSceneView>( _target->getPathPtr(),
																						  ID::View::UI_SCENE_PATH );

		pathSceneView->openRenameEditor( _target->getId() );
	}

	void ContextualMenuViewpoint::_deleteAction() { VTX_ACTION( new QT::Action::Viewpoint::Delete( *_target ) ); }
	void ContextualMenuViewpoint::_gotoAction() { VTX_ACTION( new QT::Action::Viewpoint::GoTo( *_target ) ); }
	void ContextualMenuViewpoint::_relocateAction() { VTX_ACTION( new App::Action::Viewpoint::Relocate( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
