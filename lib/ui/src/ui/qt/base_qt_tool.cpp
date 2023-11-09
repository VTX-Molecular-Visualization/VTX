#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/qt_tool_manager.hpp"

namespace VTX::UI::QT
{
	BaseQtTool::BaseQtTool() : VTX::UI::Core::BaseVTXUITool(), QObject() { QtToolManager::get().registerTool( this ); }
} // namespace VTX::UI::QT
