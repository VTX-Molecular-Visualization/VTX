#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_PATH__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_PATH__

#include "contextual_menu_template.hpp"
#include "ui/old_ui/ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>
#include <app/component/video/path.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuPath : public ContextualMenuTemplate<App::Component::Video::Path>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuPath();
		void localize() override;
		void setTarget( App::Component::Video::Path * const p_target ) override;

	  protected:
		ContextualMenuPath( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _addViewpoint();
		void _clearViewpoints();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
