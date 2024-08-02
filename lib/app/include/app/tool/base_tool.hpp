#ifndef __VTX_APP_TOOL_BASE_TOOL__
#define __VTX_APP_TOOL_BASE_TOOL__

#include <string>

namespace VTX::App::Tool
{
	class BaseTool
	{
	  public:
		// Created after the app.
		BaseTool() {}
		// Destroyed with the app.
		virtual ~BaseTool() {}

		// During app init, renderer not available.
		virtual void init() {}
		// After main window creation, renderer available.
		virtual void createUI() {}
		// After app start.
		virtual void onAppStart() {}
		// Before app stop.
		virtual void onAppStop() {}

		// TODO: pass settings system?
		virtual void loadSettings() {}
		virtual void saveSettings() {}

		// TODO: encapsulate tool settings?

	  protected:
		// Useful?
		std::string name;
	};

} // namespace VTX::App::Tool

#endif
