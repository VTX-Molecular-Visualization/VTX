#ifndef __VTX_UI_ID__
#define __VTX_UI_ID__

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
				// Base
				const VTX_ID SCENE	   = "SCENE";
				const VTX_ID RENDER	   = "RENDER";
				const VTX_ID INSPECTOR = "INSPECTOR";
				const VTX_ID CONSOLE   = "CONSOLE";
				const VTX_ID SEQUENCE  = "SEQUENCE";
				// !V0.1
				// const VTX_ID SELECTION	 = "SELECTION";
				const VTX_ID SETTINGS = "SETTINGS";

				// Tools
				const VTX_ID STRUCTURAL_ALIGNMENT = "STRUCTURAL_ALIGNMENT";

			} // namespace Window
			namespace Input
			{
				const VTX_ID MAIN_WINDOW   = "MAIN_WINDOW";
				const VTX_ID RENDER_WIDGET = "OPENGL_WIDGET";

			} // namespace Input
		}	  // namespace UI

		namespace Model
		{
			const VTX_ID MODEL_MOLECULE					= "MODEL_MOLECULE";
			const VTX_ID MODEL_CATEGORY					= "MODEL_CATEGORY";
			const VTX_ID MODEL_CHAIN					= "MODEL_CHAIN";
			const VTX_ID MODEL_RESIDUE					= "MODEL_RESIDUE";
			const VTX_ID MODEL_ATOM						= "MODEL_ATOM";
			const VTX_ID MODEL_BOND						= "MODEL_BOND";
			const VTX_ID MODEL_SECONDARY_STRUCTURE		= "MODEL_SECONDARY_STRUCTURE";
			const VTX_ID MODEL_SOLVENT_EXCLUDED_SURFACE = "MODEL_SOLVENT_EXCLUDED_SURFACE";
			const VTX_ID MODEL_GENERATED_MOLECULE		= "MODEL_GENERATED_MOLECULE";
			const VTX_ID MODEL_MESH_TRIANGLE			= "MODEL_MESH_TRIANGLE";
			const VTX_ID MODEL_PATH						= "MODEL_PATH";
			const VTX_ID MODEL_VIEWPOINT				= "MODEL_VIEWPOINT";
			const VTX_ID MODEL_SELECTION				= "MODEL_SELECTION";

			const VTX_ID MODEL_REPRESENTATION_LIBRARY	  = "MODEL_REPRESENTATION_LIBRARY";
			const VTX_ID MODEL_INTANTIATED_REPRESENTATION = "MODEL_INTANTIATED_REPRESENTATION";

			const VTX_ID MODEL_REPRESENTATION = "MODEL_REPRESENTATION";

			const VTX_ID MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY = "MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY";
			const VTX_ID MODEL_RENDERER_RENDER_EFFECT_PRESET		 = "MODEL_RENDERER_RENDER_EFFECT_PRESET";

			const VTX_ID MODEL_LABEL						   = "LABEL";
			const VTX_ID MODEL_MEASUREMENT_DISTANCE			   = "MEASUREMENT_DISTANCE";
			const VTX_ID MODEL_MEASUREMENT_DISTANCE_TO_CYCLE   = "MEASUREMENT_DISTANCE_TO_CYCLE";
			const VTX_ID MODEL_MEASUREMENT_ANGLE			   = "MEASUREMENT_ANGLE";
			const VTX_ID MODEL_MEASUREMENT_DIHEDRAL_ANGLE	   = "MEASUREMENT_DIHEDRAL_ANGLE";
			const VTX_ID MODEL_MEASUREMENT_MEASURE_IN_PROGRESS = "MODEL_MEASUREMENT_MEASURE_IN_PROGRESS";

		} // namespace Model

		namespace View
		{
			const VTX_ID INSTANTIATED_REPRESENTATION_ON_REPRESENTATION
				= "INSTANTIATED_REPRESENTATION_ON_REPRESENTATION";
			const VTX_ID REPRESENTATION_LIBRARY_ON_ITEMS = "REPRESENTATION_LIBRARY_ON_ITEMS";

			const VTX_ID RENDER_EFFECT_LIBRARY_ON_ITEMS = "RENDER_EFFECT_LIBRARY_ON_ITEMS";

			const VTX_ID MEASUREMENT_ON_MOLECULE = "MEASUREMENT_ON_MOLECULE";

			const VTX_ID UI_MOLECULE_STRUCTURE			  = "UI_MOLECULE_STRUCTURE";
			const VTX_ID UI_MOLECULE_SEQUENCE			  = "UI_MOLECULE_SEQUENCE";
			const VTX_ID UI_MOLECULE					  = "UI_MOLECULE";
			const VTX_ID UI_CHAIN						  = "UI_CHAIN";
			const VTX_ID UI_RESIDUE						  = "UI_RESIDUE";
			const VTX_ID UI_ATOM						  = "UI_ATOM";
			const VTX_ID UI_PATH_LIST					  = "UI_PATH_LIST";
			const VTX_ID UI_PATH						  = "UI_PATH";
			const VTX_ID UI_VIEWPOINT					  = "UI_VIEWPOINT";
			const VTX_ID UI_SCENE_VIEWPOINT				  = "UI_SCENE_VIEWPOINT";
			const VTX_ID UI_SCENE_PATH					  = "UI_SCENE_PATH";
			const VTX_ID UI_SELECTION					  = "UI_SELECTION";
			const VTX_ID UI_SELECTION_SEQUENCE			  = "UI_SELECTION_SEQUENCE";
			const VTX_ID UI_SCENE_REPRESENTATION		  = "UI_SCENE_REPRESENTATION";
			const VTX_ID UI_SCENE_DISTANCE_LABEL		  = "UI_SCENE_DISTANCE_LABEL";
			const VTX_ID UI_SCENE_DISTANCE_TO_CYCLE_LABEL = "UI_SCENE_DISTANCE_TO_CYCLE_LABEL";
			const VTX_ID UI_SCENE_ANGLE_LABEL			  = "UI_SCENE_ANGLE_LABEL";
			const VTX_ID UI_SCENE_DIHEDRAL_ANGLE_LABEL	  = "UI_SCENE_DIHEDRAL_ANGLE_LABEL";

			const VTX_ID UI_INSPECTOR_MOLECULE_STRUCTURE		  = "UI_INSPECTOR_MOLECULE_STRUCTURE";
			const VTX_ID UI_INSPECTOR_CHAIN						  = "UI_INSPECTOR_CHAIN";
			const VTX_ID UI_INSPECTOR_RESIDUE					  = "UI_INSPECTOR_RESIDUE";
			const VTX_ID UI_INSPECTOR_ATOM						  = "UI_INSPECTOR_ATOM";
			const VTX_ID UI_INSPECTOR_REPRESENTATION			  = "UI_INSPECTOR_REPRESENTATION";
			const VTX_ID UI_INSPECTOR_INSTANTIATED_REPRESENTATION = "UI_INSPECTOR_INSTANTIATED_REPRESENTATION";
			const VTX_ID UI_INSPECTOR_VIEWPOINT					  = "UI_INSPECTOR_VIEWPOINT";

			const VTX_ID UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY = "UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY";
			const VTX_ID UI_REPRESENTATION_LIBRARY_COMBO_BOX		  = "UI_REPRESENTATION_LIBRARY_COMBO_BOX";

			const VTX_ID UI_RENDER_EFFECT_PRESET_LIBRARY			 = "UI_RENDER_EFFECT_PRESET_LIBRARY";
			const VTX_ID UI_RENDER_EFFECT_PRESET					 = "UI_RENDER_EFFECT_PRESET";
			const VTX_ID UI_RENDER_EFFECT_LIBRARY_COMBO_BOX			 = "UI_RENDER_EFFECT_LIBRARY_COMBO_BOX";
			const VTX_ID UI_MENU_VISUALIZATION_RENDER_EFFECT_LIBRARY = "UI_MENU_VISUALIZATION_RENDER_EFFECT_LIBRARY";

			const VTX_ID UI_REPRESENTATION_PRESET_LIBRARY = "UI_REPRESENTATION_PRESET_LIBRARY";
			const VTX_ID UI_REPRESENTATION_EDITOR_PRESET  = "UI_REPRESENTATION_EDITOR_PRESET";

			const VTX_ID D3_CYLINDER	 = "3D_CYLINDER";
			const VTX_ID D3_SPHERE		 = "3D_SPHERE";
			const VTX_ID D3_BOX			 = "3D_BOX";
			const VTX_ID D3_TRIANGLE	 = "3D_TRIANGLE";
			const VTX_ID D3_RIBBON_PATCH = "3D_RIBBON_PATCH";
			const VTX_ID D3_TRIANGLE_SES = "3D_TRIANGLE_SES";

			const VTX_ID UI_RENDER_MEASUREMENT_DISTANCE			   = "UI_RENDER_MEASUREMENT_DISTANCE";
			const VTX_ID UI_RENDER_MEASUREMENT_DISTANCE_TO_CYCLE   = "UI_RENDER_MEASUREMENT_DISTANCE_TO_CYCLE";
			const VTX_ID UI_RENDER_MEASUREMENT_ANGLE			   = "UI_RENDER_MEASUREMENT_ANGLE";
			const VTX_ID UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE	   = "UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE";
			const VTX_ID UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS = "UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS";

			const VTX_ID UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT = "UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT";

		} // namespace View

		namespace State
		{
			const VTX_ID VISUALIZATION = "VISUALIZATION";
			const VTX_ID PLAY		   = "PLAY";
			const VTX_ID EXPORT		   = "EXPORT";
		} // namespace State

		namespace Controller
		{
			const VTX_ID FREEFLY	 = "FREEFLY";
			const VTX_ID TRACKBALL	 = "TRACKBALL";
			const VTX_ID ORBIT		 = "ORBIT";
			const VTX_ID SHORTCUT	 = "SHORCUT";
			const VTX_ID VESSEL		 = "VESSEL";
			const VTX_ID PICKER		 = "PICKER";
			const VTX_ID MEASUREMENT = "MEASUREMENT";
			const VTX_ID MAIN_WINDOW = "MAIN_WINDOW";
		} // namespace Controller
	}	  // namespace ID

} // namespace VTX

#endif