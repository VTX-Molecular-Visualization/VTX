#ifndef __VTX_UI_QT_TOOL_MANAGER__
#define __VTX_UI_QT_TOOL_MANAGER__

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
	  public:
		void registerTool( const BaseQtTool * const p_tool );

	  private:
		QtToolManager();
		~QtToolManager();

		std::vector<const BaseQtTool *> _tools = std::vector<const BaseQtTool *>();
	};
} // namespace VTX::UI::QT

#endif
