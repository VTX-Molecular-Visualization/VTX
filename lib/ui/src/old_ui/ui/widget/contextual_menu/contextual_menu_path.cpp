#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_path.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/view/ui/widget/path_scene_view.hpp"
#include "ui/qt/action/viewpoint.hpp"

#include <app/action/path.hpp>
#include <app/action/viewpoint.hpp>
#include <app/component/video/path.hpp>

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

	void ContextualMenuPath::setTarget( App::Component::Video::Path * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getDefaultName() ) );
	}

	void ContextualMenuPath::_addViewpoint() { VTX_ACTION( new QT::Action::Viewpoint::Create() ); }
	void ContextualMenuPath::_clearViewpoints() { VTX_ACTION( new App::Action::Path::Clear( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
