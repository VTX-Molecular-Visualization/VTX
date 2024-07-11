#ifndef __VTX_UI_QT_RESOURCES__
#define __VTX_UI_QT_RESOURCES__

#include <QString>

namespace VTX::UI::QT::Resources
{
	// Stylesheets.
	const QString FILE_STYLESHEET		 = ":/stylesheet_ui.css";
	const QString FILE_STYLESHEET_WINOWS = ":/stylesheet_ui_windows.css";
	const QString FILE_STYLESHEET_LINUX	 = ":/stylesheet_ui_linux.css";
	const QString FILE_STYLESHEET_MACOS	 = ":/stylesheet_ui_mac.css";

	//  Sprites.
	const QString SPRITE_SPLASH = ":/sprite/splash.png";
	const QString SPRITE_LOGO	= ":/sprite/logo.png";

	const QString SPRITE_CAMERA_PERSPECTIVE	 = ":/sprite/camera/perspective.png";
	const QString SPRITE_CAMERA_ORTHOGRAPHIC = ":/sprite/camera/orthographic.png";
	const QString SPRITE_CAMERA_TRACKBALL	 = ":/sprite/camera/trackball.png";
	const QString SPRITE_CAMERA_FREEFLY		 = ":/sprite/camera/freefly.png";
	const QString SPRITE_CAMERA_ORIENT		 = ":/sprite/camera/orient.png";
	const QString SPRITE_CAMERA_RESET		 = ":/sprite/camera/reset.png";

	const QString SPRITE_SNAPSHOT = ":/sprite/snapshot/snapshot.png";
} // namespace VTX::UI::QT::Resources

#endif
