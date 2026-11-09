#ifndef VTX_APP_TOOL_BASE_TOOL_HPP
#define VTX_APP_TOOL_BASE_TOOL_HPP

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
		BaseTool() = default;

		/**
		 * @brief Destroyed with the app.
		 */
		virtual ~BaseTool() = default;

		/**
		 * @brief Register optional UI actions for this tool.
		 * Called only by the Qt application
		   path before UI hooks are built.
		 */
		virtual void registerActions() {}

		/**
		 * @brief Build optional UI hooks for this tool.
		 * Called only by the Qt application path
		 * once the main window exists.
		 */
		virtual void buildUI() {}

	  protected:
		// Useful?
		std::string name;
	};

} // namespace VTX::App::Tool

#endif
