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
			namespace Window
			{
				const VTX_ID SCENE	   = "SCENE";
				const VTX_ID RENDER	   = "RENDER";
				const VTX_ID INSPECTOR = "INSPECTOR";
				const VTX_ID CONSOLE   = "CONSOLE";
				const VTX_ID SEQUENCE  = "SEQUENCE";
				const VTX_ID SELECTION = "SELECTION";
				const VTX_ID SETTINGS  = "SETTINGS";

			} // namespace Window

			const VTX_ID USER_INTERFACE = "USER_INTERFACE";
			const VTX_ID MENU			= "MENU";
			const VTX_ID CAMERA_EDITOR	= "CAMERA_EDITOR";
			const VTX_ID PROGRESS_BAR	= "PROGRESS_BAR";
			const VTX_ID INSPECTOR_ITEM = "INSPECTOR_ITEM";
		} // namespace UI

		namespace Model
		{
			const VTX_ID MODEL_MOLECULE			   = "MODEL_MOLECULE";
			const VTX_ID MODEL_CHAIN			   = "MODEL_CHAIN";
			const VTX_ID MODEL_RESIDUE			   = "MODEL_RESIDUE";
			const VTX_ID MODEL_ATOM				   = "MODEL_ATOM";
			const VTX_ID MODEL_BOND				   = "MODEL_BOND";
			const VTX_ID MODEL_SECONDARY_STRUCTURE = "MODEL_SECONDARY_STRUCTURE";
			const VTX_ID MODEL_GENERATED_MOLECULE  = "MODEL_GENERATED_MOLECULE";
			const VTX_ID MODEL_MESH_TRIANGLE	   = "MODEL_MESH_TRIANGLE";
			const VTX_ID MODEL_PATH				   = "MODEL_PATH";
			const VTX_ID MODEL_VIEWPOINT		   = "MODEL_VIEWPOINT";
			const VTX_ID MODEL_SELECTION		   = "MODEL_SELECTION";

			const VTX_ID MODEL_REPRESENTATION_LIBRARY	  = "MODEL_REPRESENTATION_LIBRARY";
			const VTX_ID MODEL_INTANTIATED_REPRESENTATION = "MODEL_INTANTIATED_REPRESENTATION";

			const VTX_ID MODEL_REPRESENTATION_BALLSANDSTICKS	  = "MODEL_REPRESENTATION_BALLSANDSTICKS";
			const VTX_ID MODEL_REPRESENTATION_STICKS			  = "MODEL_REPRESENTATION_STICKS";
			const VTX_ID MODEL_REPRESENTATION_TRACE				  = "MODEL_REPRESENTATION_TRACE";
			const VTX_ID MODEL_REPRESENTATION_VANDERWALLS		  = "MODEL_REPRESENTATION_VANDERWALLS";
			const VTX_ID MODEL_REPRESENTATION_SAS				  = "MODEL_REPRESENTATION_SAS";
			const VTX_ID MODEL_REPRESENTATION_SES				  = "MODEL_REPRESENTATION_SES";
			const VTX_ID MODEL_REPRESENTATION_CARTOON			  = "MODEL_REPRESENTATION_CARTOON";
			const VTX_ID MODEL_REPRESENTATION_STICKANDCARTOON	  = "MODEL_REPRESENTATION_STICKANDCARTOON";
			const VTX_ID MODEL_REPRESENTATION_BALLSTICKANDCARTOON = "MODEL_REPRESENTATION_BALLSTICKANDCARTOON";

			const VTX_ID MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY = "MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY";
			const VTX_ID MODEL_RENDERER_RENDER_EFFECT_PRESET		 = "MODEL_RENDERER_RENDER_EFFECT_PRESET";
		} // namespace Model

		namespace View
		{
			const VTX_ID UI_MOLECULE_STRUCTURE	 = "UI_MOLECULE_STRUCTURE";
			const VTX_ID UI_MOLECULE_SEQUENCE	 = "UI_MOLECULE_SEQUENCE";
			const VTX_ID UI_MOLECULE			 = "UI_MOLECULE";
			const VTX_ID UI_CHAIN				 = "UI_CHAIN";
			const VTX_ID UI_RESIDUE				 = "UI_RESIDUE";
			const VTX_ID UI_ATOM				 = "UI_ATOM";
			const VTX_ID UI_PATH_LIST			 = "UI_PATH_LIST";
			const VTX_ID UI_PATH				 = "UI_PATH";
			const VTX_ID UI_VIEWPOINT			 = "UI_VIEWPOINT";
			const VTX_ID UI_SELECTION			 = "UI_SELECTION";
			const VTX_ID UI_SELECTION_SEQUENCE	 = "UI_SELECTION_SEQUENCE";
			const VTX_ID UI_SCENE_REPRESENTATION = "UI_SCENE_REPRESENTATION";

			const VTX_ID UI_INSPECTOR_MOLECULE_STRUCTURE = "UI_INSPECTOR_MOLECULE_STRUCTURE";
			const VTX_ID UI_INSPECTOR_CHAIN				 = "UI_INSPECTOR_CHAIN";
			const VTX_ID UI_INSPECTOR_RESIDUE			 = "UI_INSPECTOR_RESIDUE";
			const VTX_ID UI_INSPECTOR_ATOM				 = "UI_INSPECTOR_ATOM";
			const VTX_ID UI_INSPECTOR_REPRESENTATION	 = "UI_INSPECTOR_REPRESENTATION";

			const VTX_ID UI_RENDER_EFFECT_PRESET_LIBRARY = "UI_RENDER_EFFECT_PRESET_LIBRARY";
			const VTX_ID UI_RENDER_EFFECT_PRESET		 = "UI_RENDER_EFFECT_PRESET";

			const VTX_ID D3_CYLINDER	 = "3D_CYLINDER";
			const VTX_ID D3_SPHERE		 = "3D_SPHERE";
			const VTX_ID D3_BOX			 = "3D_BOX";
			const VTX_ID D3_TRIANGLE	 = "3D_TRIANGLE";
			const VTX_ID D3_RIBBON_PATCH = "3D_RIBBON_PATCH";

		} // namespace View

		namespace State
		{
			const VTX_ID VISUALIZATION = "VISUALIZATION";
			const VTX_ID PLAY		   = "PLAY";
			const VTX_ID EXPORT		   = "EXPORT";
		} // namespace State

		namespace Controller
		{
			const VTX_ID FREEFLY   = "FREEFLY";
			const VTX_ID TRACKBALL = "TRACKBALL";
			const VTX_ID ORBIT	   = "ORBIT";
			const VTX_ID SHORTCUT  = "SHORCUT";
			const VTX_ID VESSEL	   = "VESSEL";
		} // namespace Controller
	}	  // namespace ID

} // namespace VTX

#endif
