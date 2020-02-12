#ifndef __VTX_ID__
#define __VTX_ID__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace ID
	{
		namespace UI
		{
			const std::string USER_INTERFACE = "USER_INTERFACE";
			const std::string MENU			 = "MENU";
			const std::string CONSOLE		 = "CONSOLE";
			const std::string SCENE			 = "SCENE";
			const std::string INSPECTOR		 = "INSPECTOR";
			const std::string CAMERA_EDITOR	 = "CAMERA_EDITOR";
		} // namespace UI

		namespace View
		{
			const std::string UI_MOLECULE_STRUCTURE = "UI_MOLECULE_STRUCTURE";
			const std::string UI_MOLECULE			= "UI_MOLECULE";
			const std::string UI_CHAIN				= "UI_CHAIN";
			const std::string UI_RESIDUE			= "UI_RESIDUE";
			const std::string UI_ATOM				= "UI_ATOM";
			const std::string UI_PATH_LIST			= "UI_PATH_LIST";
			const std::string UI_PATH				= "UI_PATH";
			const std::string UI_VIEWPOINT			= "UI_VIEWPOINT";

			const std::string D3_CYLINDER = "3D_CYLINDER";
			const std::string D3_SPHERE	  = "3D_SPHERE";
		} // namespace View

		namespace State
		{
			const std::string LOAD			= "LOAD";
			const std::string VISUALIZATION = "VISUALIZATION";
			const std::string PLAY			= "PLAY";
			const std::string EXPORT		= "EXPORT";
		} // namespace State

		namespace Controller
		{
			const std::string FPS	   = "FPS";
			const std::string SHORTCUT = "SHORCUT";
		} // namespace Controller
	}	  // namespace ID

} // namespace VTX

#endif
