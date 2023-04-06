#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_LABEL__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_LABEL__

#include "contextual_menu_template.hpp"
#include "model/label.hpp"
#include "ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuLabel : public ContextualMenuTemplate<Model::Label>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuLabel();
		void localize() override;
		void setTarget( Model::Label * const p_target ) override;

	  protected:
		ContextualMenuLabel( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _orientAction();
		void _renameAction();
		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
