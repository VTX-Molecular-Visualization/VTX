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
		using VTX_ID = std::string;
		namespace UI
		{
			const VTX_ID USER_INTERFACE = "USER_INTERFACE";
			const VTX_ID MENU			= "MENU";
			const VTX_ID CONSOLE		= "CONSOLE";
			const VTX_ID SCENE			= "SCENE";
			const VTX_ID INSPECTOR		= "INSPECTOR";
			const VTX_ID CAMERA_EDITOR	= "CAMERA_EDITOR";
		} // namespace UI

		namespace View
		{
			const VTX_ID UI_MOLECULE_STRUCTURE = "UI_MOLECULE_STRUCTURE";
			const VTX_ID UI_MOLECULE		   = "UI_MOLECULE";
			const VTX_ID UI_CHAIN			   = "UI_CHAIN";
			const VTX_ID UI_RESIDUE			   = "UI_RESIDUE";
			const VTX_ID UI_ATOM			   = "UI_ATOM";
			const VTX_ID UI_PATH_LIST		   = "UI_PATH_LIST";
			const VTX_ID UI_PATH			   = "UI_PATH";
			const VTX_ID UI_VIEWPOINT		   = "UI_VIEWPOINT";

			const VTX_ID D3_CYLINDER = "3D_CYLINDER";
			const VTX_ID D3_SPHERE	 = "3D_SPHERE";
		} // namespace View

		namespace State
		{
			const VTX_ID LOAD		   = "LOAD";
			const VTX_ID VISUALIZATION = "VISUALIZATION";
			const VTX_ID PLAY		   = "PLAY";
			const VTX_ID EXPORT		   = "EXPORT";
		} // namespace State

		namespace Controller
		{
			const VTX_ID FPS	  = "FPS";
			const VTX_ID SHORTCUT = "SHORCUT";
		} // namespace Controller
	}	  // namespace ID

} // namespace VTX

#endif
