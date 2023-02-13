#ifndef __VTX_UI_QT_TOOL_MANAGER__
#define __VTX_UI_QT_TOOL_MANAGER__

#include <util/filepath.hpp>
#include <vector>

namespace VTX::UI::QT
{
	class BaseQtTool;

	class QtToolManager
	{
	  public:
		inline static QtToolManager & get()
		{
			static QtToolManager instance;
			return instance;
		}

	  private:
		inline static const Util::FilePath STYLESHEET_FILE_DEFAULT = Util::FilePath( ":/stylesheet_tool.css" );

	  public:
		void registerTool( const BaseQtTool * const p_tool );

	  private:
		QtToolManager();
		~QtToolManager();

		std::vector<const BaseQtTool *> _tools = std::vector<const BaseQtTool *>();
	};
} // namespace VTX::UI::QT

#endif
