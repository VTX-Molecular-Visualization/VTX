#ifndef __VTX_UI_ID__
#define __VTX_UI_ID__

#include <app/old_app/id.hpp>

// TODO rename this namespace to UI when all UI will be extracted from src (and remove UI IDs in src/id.hpp
namespace VTX::ID::UI_NEW
{
	namespace Window
	{
		// Base
		const VTX::ID::VTX_ID SCENE		= "SCENE";
		const VTX::ID::VTX_ID RENDER	= "RENDER";
		const VTX::ID::VTX_ID INSPECTOR = "INSPECTOR";
		const VTX::ID::VTX_ID CONSOLE	= "CONSOLE";
		const VTX::ID::VTX_ID SEQUENCE	= "SEQUENCE";
		// !V0.1
		// const VTX::ID::VTX_ID SELECTION	 = "SELECTION";
		const VTX::ID::VTX_ID SETTINGS	  = "SETTINGS";
		const VTX::ID::VTX_ID INFORMATION = "INFORMATION";

		// Tools
		const VTX::ID::VTX_ID STRUCTURAL_ALIGNMENT = "STRUCTURAL_ALIGNMENT";

	} // namespace Window
	namespace Input
	{
		const VTX::ID::VTX_ID MAIN_WINDOW	= "MAIN_WINDOW";
		const VTX::ID::VTX_ID RENDER_WIDGET = "OPENGL_WIDGET";

	} // namespace Input

	namespace State
	{
		const VTX::ID::VTX_ID VISUALIZATION = "VISUALIZATION";
		const VTX::ID::VTX_ID PLAY			= "PLAY";
		const VTX::ID::VTX_ID EXPORT		= "EXPORT";
	} // namespace State

	namespace Controller
	{
		const VTX::ID::VTX_ID FREEFLY	  = "FREEFLY";
		const VTX::ID::VTX_ID TRACKBALL	  = "TRACKBALL";
		const VTX::ID::VTX_ID ORBIT		  = "ORBIT";
		const VTX::ID::VTX_ID SHORTCUT	  = "SHORCUT";
		const VTX::ID::VTX_ID VESSEL	  = "VESSEL";
		const VTX::ID::VTX_ID PICKER	  = "PICKER";
		const VTX::ID::VTX_ID MEASUREMENT = "MEASUREMENT";
		const VTX::ID::VTX_ID MAIN_WINDOW = "MAIN_WINDOW";
	} // namespace Controller
} // namespace VTX::ID::UI_NEW

#endif
