#ifndef __VTX_UI_QT_STYLE__
#define __VTX_UI_QT_STYLE__

#include <app/tool/base_tool.hpp>
#include <vector>

namespace VTX::UI::QT
{
	/**
	 * @brief Class managing application style (themes, stylesheets, etc.).
	 */
	class Style
	{
	  public:
		/**
		 * @brief Available themes.
		 */
		enum E_THEME
		{
			SYSTEM,
			LIGHT,
			DARK,
			COUNT
		};

		/**
		 * @brief Apply all design elements.
		 */
		void load( const std::vector<App::Tool::BaseTool *> & );

		/**
		 * @brief Switch application theme.
		 */
		void setTheme( const E_THEME p_theme );

		/**
		 * @brief Get current theme.
		 */
		inline E_THEME getCurrentTheme() const { return _currentTheme; }

	  private:
		E_THEME _currentTheme = E_THEME::SYSTEM;
	};
} // namespace VTX::UI::QT

#endif
