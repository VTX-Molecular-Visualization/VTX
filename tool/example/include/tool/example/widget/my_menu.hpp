#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__

#include "tool/example/actions.hpp"
#include <QMenu>
#include <ui/qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyMenu : public UI::QT::Core::BaseWidget<MyMenu, QMenu>
	{
	  public:
		MyMenu( QWidget * p_parent ) : BaseWidget<MyMenu, QMenu>( "My menu", p_parent )
		{
			setObjectName( "ToolExampleMenu" );

			addAction<Action::MyAction1>();
			addSeparator();
			addAction<Action::MyAction2>();
			addAction<Action::MyAction3>();
			addSeparator();
			addAction<Action::MyAction4>();
		}
		virtual ~MyMenu() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
