#include "action.hpp"
#include <util/logger.hpp>

namespace VTX::Test::ExternalTool::Action
{
	namespace
	{
		bool g_toolActionExecuted = false;
	}
	void ToolAction::reset() noexcept { g_toolActionExecuted = false; }
	bool ToolAction::executed() noexcept { return g_toolActionExecuted; }
	void ToolAction::execute() { g_toolActionExecuted = true; }

} // namespace VTX::Test::ExternalTool::Action
