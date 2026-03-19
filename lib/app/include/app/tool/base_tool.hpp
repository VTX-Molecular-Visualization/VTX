#ifndef __VTX_APP_TOOL_BASE_TOOL__
#define __VTX_APP_TOOL_BASE_TOOL__

#include <optional>
#include <string>

namespace VTX::App::Tool
{
	/**
	 * @brief Abstract class that define tool behaviour.
	 */
	class BaseTool
	{
	  public:
		/**
		 * @brief Constructed after the app.
		 */
		BaseTool() {}

		/**
		 * @brief Destroyed with the app.
		 */
		virtual ~BaseTool() {}

		/**
		 * @brief Build optional UI hooks for this tool.
		 * Called only by the Qt application path once the main window exists.
		 */
		virtual void buildUI() {}

	  protected:
		// Useful?
		std::string name;
	};

} // namespace VTX::App::Tool

#endif
