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
		 * @brief Called during app initialization, renderer not available.
		 */
		virtual void init() = 0;
		/**
		 * @brief Called after main window creation, renderer available.
		 */
		virtual void createUI() {}
		/**
		 * @brief Get the style (loaded from resource or raw string).
		 * @return the stylesheet to apply.
		 */
		virtual std::optional<std::string> getStyle() const { return std::nullopt; }
		/**
		 * @brief Called when the app starts.
		 */
		virtual void onAppStart() {}
		/**
		 * @brief Called before the app stops.
		 */
		virtual void onAppStop() {}

		virtual void loadSettings() {}
		virtual void saveSettings() {}

		// TODO: encapsulate tool settings?

	  protected:
		// Useful?
		std::string name;
	};

} // namespace VTX::App::Tool

#endif
