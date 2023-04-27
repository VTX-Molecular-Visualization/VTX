#ifndef __VTX_APP_EVENT_GLOBAL__
#define __VTX_APP_EVENT_GLOBAL__

#include "app/core/event/vtx_event.hpp"

namespace VTX::App::Event
{
	enum Global : Core::Event::VTX_EVENT
	{
		UI_APPLICATION_INITIALIZED,
		LOG_CONSOLE,
		CLEAR_CONSOLE,
		CHANGE_STATE,
		UPDATE_PROGRESS_BAR,
		SCENE_ITEM_ADDED,
		SCENE_ITEM_REMOVED,
		MOLECULE_ADDED,
		MOLECULE_REMOVED,
		MOLECULE_STRUCTURE_CHANGE,
		MOLECULE_COLOR_CHANGE,
		MOLECULE_ELEMENT_DISPLAY_CHANGE,
		CHAIN_REMOVED,
		RESIDUE_REMOVED,
		ATOM_REMOVED,
		MESH_ADDED,
		MESH_REMOVED,
		SELECTION_ADDED,
		SELECTION_REMOVED,
		SELECTION_CHANGE,
		LABEL_ADDED,
		LABEL_REMOVED,
		CURRENT_ITEM_IN_SELECTION_CHANGE,
		REPRESENTATION_ADDED,
		REPRESENTATION_REMOVED,
		RENDER_EFFECT_ADDED,
		RENDER_EFFECT_REMOVED,
		RENDER_EFFECT_LIBRARY_CLEARED,
		APPLIED_RENDER_EFFECT_CHANGE,
		VIEWPOINT_ADDED,
		VIEWPOINT_REMOVED,
		PATH_ADDED,
		PATH_REMOVED,
		CONTROLLER_CHANGE,
		PICKER_MODE_CHANGE,
		DOCK_WINDOW_VISIBILITY_CHANGE,
		MAIN_WINDOW_MODE_CHANGE,
		RENDER_OVERLAY_VISIBILITY_CHANGE,
		LATE_UPDATE,
		RECENT_FILES_CHANGE,
		RECENT_DOWNLOAD_CODE_CHANGE,
		SETTINGS_CHANGE,
		SCENE_ITEM_INDEXES_CHANGE,
		SCENE_SAVED,
		SCENE_MODIFICATION_STATE_CHANGE,
		SCENE_PATH_CHANGE,
		RMSD_COMPUTED,
		STRUCTURAL_ALIGNMENT_COMPUTED,
	};

} // namespace VTX::App::Event
#endif