#include "ui/qt/qt_tool_manager.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/main_window.hpp"
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
