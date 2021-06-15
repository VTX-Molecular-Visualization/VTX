#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_TEMPLATE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_TEMPLATE__

#include "ui/widget/base_manual_widget.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	template<typename T>
	class ContextualMenuTemplate : public BaseManualWidget<QMenu>
	{
		VTX_WIDGET

	  public:
		virtual void setTarget( T * const p_target ) { _target = p_target; }

	  protected:
		ContextualMenuTemplate( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {}

	  protected:
		T * _target = nullptr;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
