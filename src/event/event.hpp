#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#ifdef _MSC_VER
#pragma once
#endif

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
			CHANGE_STATE,
			UPDATE_PROGRESS_BAR,
			MOLECULE_CREATED,
			MOLECULE_ADDED,
			MOLECULE_REMOVED,
			MESH_CREATED,
			MESH_ADDED,
			MESH_REMOVED,
			SELECTION_ADDED,
			SELECTION_REMOVED,
			SELECTION_CHANGE,
			REPRESENTATION_ADDED,
			REPRESENTATION_REMOVED,
			CONTROLLER_CHANGE,
			DOCK_WINDOW_VISIBILITY_CHANGE

		};

		// Model events for notifier pattern (model->views)[1-n].
		enum Model
		{
			INIT,
			DATA_CHANGE,
			MOLECULE_VISIBILITY,
			CHAIN_VISIBILITY,
			RESIDUE_VISIBILITY,
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
			VTXEventLog( const VTX_EVENT & p_event, const std::string & p_level, const std::string & p_date, const std::string & p_message ) :
				VTXEvent( p_event ), level( p_level ), date( p_date ), message( p_message )
			{
			}
			std::string level;
			std::string date;
			std::string message;
		};

	} // namespace Event
} // namespace VTX
#endif
