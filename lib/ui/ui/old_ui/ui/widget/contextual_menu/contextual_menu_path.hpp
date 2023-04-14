#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_PATH__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_PATH__

#include "contextual_menu_template.hpp"
#include "old_ui/ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>
#include <app/old_app/model/path.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuPath : public ContextualMenuTemplate<Model::Path>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuPath();
		void localize() override;
		void setTarget( Model::Path * const p_target ) override;

	  protected:
		ContextualMenuPath( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _addViewpoint();
		void _clearViewpoints();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
