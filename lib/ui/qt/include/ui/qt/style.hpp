#ifndef __VTX_UI_QT_STYLE__
#define __VTX_UI_QT_STYLE__

#include <QFontDatabase>
#include <QPalette>
#include <app/tool/base_tool.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::UI::QT
{

	/**
	 * @brief Class managing application style (themes, stylesheets, etc.).
	 */
	class Style
	{
	  public:
		Style();
		~Style();

		/**
		 * @brief Available themes.
		 */
		enum struct E_THEME : uint8_t
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

		/**
		 * @brief Get the available fonts.
		 */
		inline QStringList getAvailableFonts() const { return QFontDatabase::families(); }

	  private:
		/**
		 * @brief Current theme.
		 */
		E_THEME _currentTheme;

		/**
		 * @brief Store palettes for each theme.
		 */
		std::array<QPalette, toUnderlying( E_THEME::COUNT )> _themePalettes;
	};
} // namespace VTX::UI::QT

#endif
