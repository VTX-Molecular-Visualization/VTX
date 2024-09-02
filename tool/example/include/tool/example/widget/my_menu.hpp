#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_MENU__

#include "my_dialog.hpp"
#include "tool/example/actions.hpp"
#include <QMenu>
#include <ui/qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyMenu : public UI::QT::BaseWidget<MyMenu, QMenu>
	{
	  public:
		MyMenu( QWidget * p_parent ) : BaseWidget<MyMenu, QMenu>( "My menu", p_parent )
		{
			setObjectName( "ToolExampleMenu" );

			// Connect to dialog.
			auto * action1 = addAction<Action::MyAction1>();
			connect(
				action1,
				&QAction::triggered,
				this,
				[]()
				{
					MyDialog dialog;
					dialog.exec();
				}
			);

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
