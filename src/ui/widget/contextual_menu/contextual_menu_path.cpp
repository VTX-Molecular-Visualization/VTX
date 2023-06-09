#include "contextual_menu_path.hpp"
#include "action/action_manager.hpp"
#include "action/path.hpp"
#include "action/viewpoint.hpp"
#include "model/path.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/path_scene_view.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuPath::ContextualMenuPath( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuPath::~ContextualMenuPath() {}

	void ContextualMenuPath::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuPath::_setupSlots()
	{
		addSection( "Action" );
		addAction( "Add Viewpoint", this, &ContextualMenuPath::_addViewpoint );
		addAction( "Clear All", this, &ContextualMenuPath::_clearViewpoints );
	}

	void ContextualMenuPath::localize() {}

	void ContextualMenuPath::setTarget( Model::Path * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getDefaultName() ) );
	}

	void ContextualMenuPath::_addViewpoint() { VTX_ACTION( new Action::Viewpoint::Create() ); }
	void ContextualMenuPath::_clearViewpoints() { VTX_ACTION( new Action::Path::Clear( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
