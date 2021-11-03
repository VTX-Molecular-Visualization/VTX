#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_VIEWPOINT__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_VIEWPOINT__

#include "contextual_menu_template.hpp"
#include "model/viewpoint.hpp"
#include "ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuViewpoint : public ContextualMenuTemplate<Model::Viewpoint>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuViewpoint();
		void localize() override;
		void setTarget( Model::Viewpoint * const p_target ) override;

	  protected:
		ContextualMenuViewpoint( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _gotoAction();
		void _renameAction();
		void _relocateAction();
		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
