#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#include <string>

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace Event
	{
		// Global system events [n-n].
		using VTX_EVENT = int;

		enum Global
		{
			LOG_CONSOLE,
			CLEAR_CONSOLE,
			CHANGE_STATE,
			UPDATE_PROGRESS_BAR,
			MODEL_REMOVED,
			MOLECULE_CREATED,
			MOLECULE_ADDED,
			MOLECULE_REMOVED,
			MOLECULE_STRUCTURE_CHANGE,
			MOLECULE_COLOR_CHANGE,
			MOLECULE_ELEMENT_DISPLAY_CHANGE,
			CHAIN_REMOVED,
			RESIDUE_REMOVED,
			ATOM_REMOVED,
			MESH_CREATED,
			MESH_ADDED,
			MESH_REMOVED,
			SELECTION_ADDED,
			SELECTION_REMOVED,
			SELECTION_CHANGE,
			LABEL_ADDED,
			LABEL_REMOVED,
			HELPER_ADDED,
			HELPER_REMOVED,
			CURRENT_ITEM_IN_SELECTION_CHANGE,
			REPRESENTATION_ADDED,
			REPRESENTATION_REMOVED,
			RENDER_EFFECT_ADDED,
			RENDER_EFFECT_REMOVED,
			RENDER_EFFECT_LIBRARY_CLEARED,
			VIEWPOINT_ADDED,
			VIEWPOINT_REMOVED,
			PATH_ADDED,
			PATH_REMOVED,
			CONTROLLER_CHANGE,
			PICKER_MODE_CHANGE,
			DOCK_WINDOW_VISIBILITY_CHANGE,
			MAIN_WINDOW_MODE_CHANGE,
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

		// Model events for notifier pattern (model->views)[1-n].
		enum Model
		{
			INIT,
			DATA_CHANGE,
			MOLECULE_VISIBILITY,
			CHAIN_VISIBILITY,
			RESIDUE_VISIBILITY,
			ATOM_VISIBILITY,
			VISIBILITY,
			TRANSFORM_CHANGE,
			REPRESENTATION_CHANGE,
			COLOR_CHANGE,
			DISPLAY_NAME_CHANGE,
			QUICK_ACCESS_CHANGE,
			SUBITEM_DATA_CHANGE,
			REPRESENTATION_TYPE_CHANGE,
			TRAJECTORY_FRAME_CHANGE,
			TRAJECTORY_DATA_CHANGE,
			AUTO_ROTATE_DATA_CHANGE,
			APPLIED_PRESET_CHANGE,
		};

		// Base event without args.
		struct VTXEvent
		{
			VTXEvent( const VTX_EVENT & p_event ) : name( p_event ) {}
			virtual ~VTXEvent() = default;
			VTX_EVENT name;
		};

		// Templated events with args.
		template<typename T>
		struct VTXEventValue : public VTXEvent
		{
			VTXEventValue( const VTX_EVENT & p_event, const T & p_value ) : VTXEvent( p_event ), value( p_value ) {}
			T value;
		};

		template<typename T>
		struct VTXEventRef : public VTXEvent
		{
			VTXEventRef( const VTX_EVENT & p_event, T & p_ref ) : VTXEvent( p_event ), ref( p_ref ) {}
			T & ref;
		};

		template<typename T>
		struct VTXEventPtr : public VTXEvent
		{
			VTXEventPtr( const VTX_EVENT & p_event, T * const p_ptr ) : VTXEvent( p_event ), ptr( p_ptr ) {}
			T * const ptr;
		};

		// Other events.
		struct VTXEventLog : public VTXEvent
		{
			VTXEventLog( const VTX_EVENT &	 p_event,
						 const std::string & p_level,
						 const std::string & p_date,
						 const std::string & p_message ) :
				VTXEvent( p_event ),
				level( p_level ), date( p_date ), message( p_message )
			{
			}
			std::string level;
			std::string date;
			std::string message;
		};

	} // namespace Event
} // namespace VTX
#endif
