#include "base_qt_tool.hpp"
#include "qt_tool_manager.hpp"

namespace VTX::Tool::QT
{
	BaseQtTool::BaseQtTool() : VTX::UI::Core::BaseVTXUITool(), QObject() { QtToolManager::get().registerTool( this ); }
} // namespace VTX::Tool::QT
