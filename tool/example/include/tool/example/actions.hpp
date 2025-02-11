#ifndef __VTX_TOOL_EXAMPLE_ACTIONS_
#define __VTX_TOOL_EXAMPLE_ACTIONS_

#include "widget/my_dialog.hpp"
#include <app/ui/concepts.hpp>

namespace VTX::Tool::Example::Action
{

	struct MyAction1 : public App::UI::DescAction
	{
		MyAction1()
		{
			name	= "Open dialog";
			tip		= "Demo action";
			trigger = []()
			{
				Widget::MyDialog dialog;
				dialog.exec();
			};
		}
	};

	struct MyAction2 : public App::UI::DescAction
	{
		MyAction2()
		{
			name = "My action 2";
			tip	 = "Demo action";
		}
	};

	struct MyAction3 : public App::UI::DescAction
	{
		MyAction3()
		{
			name = "My action 3";
			tip	 = "Demo action";
		}
	};

	struct MyAction4 : public App::UI::DescAction
	{
		MyAction4()
		{
			name = "My action 4";
			tip	 = "Demo action";
		}
	};

} // namespace VTX::Tool::Example::Action

#endif
