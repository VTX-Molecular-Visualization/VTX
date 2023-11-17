#include "action.hpp"
#include <util/logger.hpp>

namespace VTX::Test::ExternalTool::Action
{
	void ToolAction::execute() { VTX_WARNING( "C'est qui le roi des trucmuches alors hein ?" ); }
} // namespace VTX::Test::ExternalTool::Action
