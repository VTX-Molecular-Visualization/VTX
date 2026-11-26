#ifndef VTX_UI_QT_STYLE_STYLE_MANAGER_HPP
#define VTX_UI_QT_STYLE_STYLE_MANAGER_HPP

#include "ui/qt/application.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/style/codepoint_icon_engine.hpp"
#include <QFontDatabase>
#include <QPalette>
#include <app/tool/base_tool.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::UI::QT::Style
{

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
	 * @brief Default values.
	 */
	constexpr float			  DEFAULT_SIZE_SCALE = 0.85f;
	constexpr int			  DEFAULT_FONT_SIZE	 = 10;
	constexpr E_THEME		  DEFAULT_THEME		 = E_THEME::DARK;
	constexpr QUtf8StringView DEFAULT_FONT_FAMILY { Resources::FONT_CASCADIA_CODE_FAMILY };
	constexpr QUtf8StringView DEFAULT_FONT_FAMILY_SEQUENCE = { Resources::FONT_LIBERATION_MONO_FAMILY };
	constexpr QUtf8StringView DYSLEXIC_FONT_FAMILY		   = { Resources::FONT_OPEN_DYSLEXIC_FAMILY };
	constexpr int			  MINIMUM_DOCK_PANEL_WIDTH	   = 200;
	constexpr int			  MINIMUM_DOCK_PANEL_HEIGHT	   = 50;
	constexpr int			  DEFAULT_LATERAL_DOCK_WIDTH   = 250;
	constexpr int			  DEFAULT_CONSOLE_HEIGHT	   = 120;

	constexpr int HUE_GRAPHICS_CONFIG = 260;
	constexpr int HUE_COLOR_LAYOUT	  = 30;
	constexpr int HUE_REPRESENTATION  = 320;
	constexpr int HUE_SCRIPTS		  = 100;
	constexpr int HUE_CAMERA		  = 180;
	constexpr int HUE_SYSTEM		  = 215;

	/**
	 * @brief Class managing application style (themes, stylesheets, etc.).
	 */
	class StyleManager
	{
	  public:
		StyleManager();
		StyleManager( const StyleManager & )			 = delete;
		StyleManager & operator=( const StyleManager & ) = delete;
		~StyleManager();

		/**
		 * @brief Get current theme.
		 */
		E_THEME getCurrentTheme() const { return _currentTheme; }

		/**
		 * @brief Get current font family.
		 */
		QString getCurrentFontFamily() const { return Q_APP()->font().family(); }

		/**
		 * @brief Switch application theme.
		 */
		void setTheme( E_THEME p_theme );

		/**
		 * @brief Set font.
		 */
		void setFontFamily( QAnyStringView );

		/**
		 * @brief Get an icon from a font.
		 */
		QIcon iconFromCodepoint( Codepoint );

		const QStringList & getBundledFonts() const { return _bundledFonts; }

		/**
		 * @brief Get the available fonts.
		 */
		QStringList getSystemFonts() const;

	  private:
		/**
		 * @brief Current theme.
		 */
		E_THEME _currentTheme = DEFAULT_THEME;

		/**
		 * @brief Store palettes for each theme.
		 */
		std::array<QPalette, toUnderlying( E_THEME::COUNT )> _themePalettes;

		QStringList _bundledFonts;
	};
} // namespace VTX::UI::QT::Style

#endif
