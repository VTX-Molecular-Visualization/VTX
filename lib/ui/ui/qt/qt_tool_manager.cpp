#include "qt_tool_manager.hpp"
#include "base_qt_tool.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include <qglobal.h>

namespace VTX::UI::QT
{
	QtToolManager::QtToolManager()
	{
		Q_INIT_RESOURCE( resources_tool );
		UI::QT::QT_APP()->getMainWindow().appendStylesheet( STYLESHEET_FILE_DEFAULT.string().c_str() );
	}
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
