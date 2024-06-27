#ifndef __VTX_UI_QT_RESOURCES__
#define __VTX_UI_QT_RESOURCES__

#include <QString>

namespace VTX::UI::QT::Resources
{
	constexpr QStringView APPLICATION_DISPLAY_NAME = u"VTX";
	constexpr QStringView APPLICATION_NAME		   = u"VTX";
	constexpr QStringView ORGANIZATION_NAME		   = u"VTX Consortium";
	constexpr QStringView ORGANIZATION_DOMAIN	   = u"vtx.drugdesign.fr";
	// TODO: use App constants.
	constexpr QStringView APPLICATION_VERSION = u"1.0";

	// Stylesheets.
	constexpr QStringView FILE_STYLESHEET		 = u":/stylesheet_ui.css";
	constexpr QStringView FILE_STYLESHEET_WINOWS = u":/stylesheet_ui_windows.css";
	constexpr QStringView FILE_STYLESHEET_LINUX	 = u":/stylesheet_ui_linux.css";
	constexpr QStringView FILE_STYLESHEET_MACOS	 = u":/stylesheet_ui_mac.css";

	//  Sprites.
	constexpr QStringView SPRITE_SPLASH = u":/sprite/splash.png";
	constexpr QStringView SPRITE_LOGO	= u":/sprite/logo.png";
} // namespace VTX::UI::QT::Resources

#endif
