#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__

#include "tool/example/actions.hpp"
#include <QMenu>
#include <ui/qt/widget/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyMenu : public UI::QT::Widget::BaseWidget<MyMenu, QMenu>
	{
	  public:
		MyMenu( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "My Menu" );

			addAction( Action::MY_ACTION_1 );
			addSeparator();
			addAction( Action::MY_ACTION_2 );
			addAction( Action::MY_ACTION_3 );
			addSeparator();
			addAction( Action::MY_ACTION_4 );
		}
		virtual ~MyMenu() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
