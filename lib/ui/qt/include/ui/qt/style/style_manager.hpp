#ifndef __VTX_UI_QT_STYLE_STYLE_MANAGER__
#define __VTX_UI_QT_STYLE_STYLE_MANAGER__

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
	constexpr float	  DEFAULT_SIZE_SCALE		   = 0.85f;
	constexpr int	  DEFAULT_FONT_SIZE			   = 10;
	constexpr E_THEME DEFAULT_THEME				   = E_THEME::DARK;
	const QString	  DEFAULT_FONT_FAMILY		   = Resources::FONT_CASCADIA_MONO_FAMILY.data();
	const QString	  DEFAULT_FONT_FAMILY_SEQUENCE = Resources::FONT_LIBERATION_MONO_FAMILY.data();
	constexpr int	  MINIMUM_DOCK_PANEL_WIDTH	   = 200;
	constexpr int	  MINIMUM_DOCK_PANEL_HEIGHT	   = 50;
	constexpr int	  DEFAULT_LATERAL_DOCK_WIDTH   = 250;
	constexpr int	  DEFAULT_CONSOLE_HEIGHT	   = 120;

	/**
	 * @brief Class managing application style (themes, stylesheets, etc.).
	 */
	class StyleManager
	{
	  public:
		StyleManager();
		~StyleManager();

		/**
		 * @brief Get current theme.
		 */
		inline E_THEME getCurrentTheme() const { return _currentTheme; }

		/**
		 * @brief Get current font family.
		 */
		inline QString getCurrentFontFamily() const { return Q_APP()->font().family(); }

		/**
		 * @brief Switch application theme.
		 */
		void setTheme( const E_THEME p_theme );

		/**
		 * @brief Set font.
		 */
		void setFontFamily( const QString & );

		/**
		 * @brief Get an icon from a font.
		 */
		QIcon iconFromCodepoint( const Codepoint );

		/**
		 * @brief Get the available fonts.
		 */
		inline static QStringList getAvailableFonts() { return QFontDatabase::families(); }

	  private:
		/**
		 * @brief Current theme.
		 */
		E_THEME _currentTheme = DEFAULT_THEME;

		/**
		 * @brief Store palettes for each theme.
		 */
		std::array<QPalette, toUnderlying( E_THEME::COUNT )> _themePalettes;
	};
} // namespace VTX::UI::QT::Style

#endif
