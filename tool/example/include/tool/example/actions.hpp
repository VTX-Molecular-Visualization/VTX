#ifndef __VTX_TOOL_EXAMPLE_ACTIONS_
#define __VTX_TOOL_EXAMPLE_ACTIONS_

#include <app/ui/concepts.hpp>
#include <string_view>

namespace VTX::Tool::Example::Action
{
	inline constexpr std::string_view MY_ACTION_1 = "example.myAction1";
	inline constexpr std::string_view MY_ACTION_2 = "example.myAction2";
	inline constexpr std::string_view MY_ACTION_3 = "example.myAction3";
	inline constexpr std::string_view MY_ACTION_4 = "example.myAction4";

	inline App::UI::DescAction myAction1()
	{
		App::UI::DescAction action;
		action.key	= MY_ACTION_1;
		action.name = "Open dialog";
		action.tip	= "Demo action";
		return action;
	}

	inline App::UI::DescAction myAction2()
	{
		App::UI::DescAction action;
		action.key	= MY_ACTION_2;
		action.name = "My action 2";
		action.tip	= "Demo action";
		return action;
	}

	inline App::UI::DescAction myAction3()
	{
		App::UI::DescAction action;
		action.key	= MY_ACTION_3;
		action.name = "My action 3";
		action.tip	= "Demo action";
		return action;
	}

	inline App::UI::DescAction myAction4()
	{
		App::UI::DescAction action;
		action.key	= MY_ACTION_4;
		action.name = "My action 4";
		action.tip	= "Demo action";
		return action;
	}

} // namespace VTX::Tool::Example::Action

#endif
