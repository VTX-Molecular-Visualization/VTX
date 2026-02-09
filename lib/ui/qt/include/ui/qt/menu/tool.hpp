#ifndef __VTX_UI_QT_MENU_TOOL__
#define __VTX_UI_QT_MENU_TOOL__

#include "ui/qt/helper.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Tool : public Widget::BaseWidget<Tool, QMenu>
	{
	  public:
		Tool( QWidget * p_parent ) : BaseWidget( p_parent ) { setTitle( "Tool" ); }

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
