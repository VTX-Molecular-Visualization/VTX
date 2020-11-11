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
		using VTX_EVENT = std::string;

		namespace Global
		{
			const VTX_EVENT LOG_CONSOLE			= "LOG_CONSOLE";
			const VTX_EVENT CHANGE_STATE		= "CHANGE_STATE";
			const VTX_EVENT UPDATE_PROGRESS_BAR = "UPDATE_PROGRESS_BAR";
			const VTX_EVENT MOLECULE_ADDED		= "MOLECULE_ADDED";
			const VTX_EVENT MOLECULE_REMOVED	= "MOLECULE_REMOVED";
			const VTX_EVENT MESH_ADDED			= "MESH_ADDED";
			const VTX_EVENT MESH_REMOVED		= "MESH_REMOVED";
			const VTX_EVENT SELECTION_ADDED		= "SELECTION_ADDED";
			const VTX_EVENT SELECTION_REMOVED	= "SELECTION_REMOVED";
			const VTX_EVENT SELECTION_CHANGE	= "SELECTION_CHANGE";
			const VTX_EVENT CONTROLLER_CHANGE	= "CONTROLLER_CHANGE";

		} // namespace Global

		namespace UI
		{
			const VTX_EVENT DOCK_WINDOW_VISIBILITY_CHANGE = "DOCK_WINDOW_VISIBILITY_CHANGE";
		}

		// Model events for notifier pattern (model->views)[1-n].
		enum class VTX_EVENT_MODEL : int
		{
			INIT,
			RENDER,
			DATA_CHANGE,
			CHILD_DATA_CHANGE,
			
		};

		struct VTXEventModelData
		{
		  public:
			VTXEventModelData() {};
			virtual ~VTXEventModelData() = default;
		};

		template<typename T>
		struct VTXEventModelDataTemplated : public VTXEventModelData
		{
		  public:
			VTXEventModelDataTemplated( const T & p_value ) : VTXEventModelData(), value( p_value ) {};
			const T & value;
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
