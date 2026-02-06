#ifndef __VTX_UI_QT_STYLE_CODEPOINT_ICON_ENGIN__
#define __VTX_UI_QT_STYLE_CODEPOINT_ICON_ENGIN__

#include <QIconEngine>

namespace VTX::UI::QT::Style
{
	/**
	 * @brief Type alias for codepoints.
	 */
	using Codepoint = int;

	/**
	 * @brief Default font family for icons.
	 */
	const QString DEFAULT_FONT_FAMILY_ICONS = "Material Symbols Outlined";

	/**
	 * @brief Custom QIconEngine to repaint icons with the current theme color.
	 */
	class CodepointIconEngine final : public QIconEngine
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		CodepointIconEngine( const int p_codepoint ) : _codepoint( p_codepoint ) {}

		/**
		 * @brief Override: clone the engine.
		 */
		inline QIconEngine * clone() const override { return new CodepointIconEngine( _codepoint ); }

		/**
		 * @brief Override: paint the icon using the current theme color.
		 */
		void paint( QPainter * p_painter, const QRect & p_rect, QIcon::Mode p_mode, QIcon::State p_state ) override;

		/**
		 * @brief Override: return the pixmap for the icon.
		 */
		QPixmap pixmap( const QSize &, QIcon::Mode, QIcon::State ) override;

	  private:
		/**
		 * @brief The codepoint to draw.
		 */
		const Codepoint _codepoint;
	};
} // namespace VTX::UI::QT::Style

#endif
