#ifndef __VTX_UI_QT_TOOL_CONSOLE_ACTION_CONSOLE__
#define __VTX_UI_QT_TOOL_CONSOLE_ACTION_CONSOLE__

#include <app/core/action/base_action.hpp>

namespace VTX::UI::QT::Tool::Console::Action
{
	class ClearConsole final : public App::Core::Action::BaseAction
	{
	  public:
		ClearConsole() {}
		void execute() override;
	};

} // namespace VTX::UI::QT::Tool::Console::Action
#endif
