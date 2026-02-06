#ifndef __VTX_UI_QT_STYLE__
#define __VTX_UI_QT_STYLE__

#include "ui/qt/application.hpp"
#include <QFontDatabase>
#include <QIcon>
#include <QIconEngine>
#include <QPainter>
#include <QPalette>
#include <app/tool/base_tool.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::UI::QT
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
	constexpr int	  DEFAULT_FONT_SIZE			= 10;
	constexpr E_THEME DEFAULT_THEME				= E_THEME::SYSTEM;
	const QString	  DEFAULT_FONT_FAMILY		= "Consolas";
	const QString	  DEFAULT_FONT_FAMILY_ICONS = "Material Symbols Outlined";
	constexpr int	  DEFAULT_ICON_SIZE			= 64;

	/**
	 * @brief Class managing application style (themes, stylesheets, etc.).
	 */
	class Style
	{
	  public:
		Style();
		~Style();

		/**
		 * @brief Apply all design elements.
		 */
		void load( const std::vector<App::Tool::BaseTool *> & );

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
		QIcon iconFromCodepoint( const int );

		/**
		 * @brief Get the available fonts.
		 */
		inline static QStringList getAvailableFonts() { return QFontDatabase::families(); }

		/**
		 * @brief Custom QIconEngine to repaint icons with the current theme color.
		 */
		class CodepointIconEngine final : public QIconEngine
		{
		  public:
			CodepointIconEngine( const int p_codepoint ) : _codepoint( p_codepoint ) {}
			inline QIconEngine * clone() const override { return new CodepointIconEngine( _codepoint ); }
			void paint( QPainter * p_painter, const QRect & p_rect, QIcon::Mode p_mode, QIcon::State p_state ) override;
			QPixmap pixmap( const QSize &, QIcon::Mode, QIcon::State ) override;

		  private:
			const int _codepoint;
		};

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
