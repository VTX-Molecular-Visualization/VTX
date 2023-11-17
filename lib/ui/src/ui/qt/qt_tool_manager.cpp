#include "ui/qt/qt_tool_manager.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT
{
	QtToolManager::QtToolManager() {}
	QtToolManager::~QtToolManager()
	{
		for ( const BaseQtTool * const tool : _tools )
		{
			delete tool;
		}

		_tools.clear();
		_tools.shrink_to_fit();
	}

	void QtToolManager::registerTool( const BaseQtTool * const p_tool ) { _tools.emplace_back( p_tool ); }

} // namespace VTX::UI::QT
