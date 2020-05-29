#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Event
	{
		// Global system events [n-n].
		using VTX_EVENT = std::string;

		namespace Global
		{
			const VTX_EVENT LOG_CONSOLE			= "LOG_CONSOLE";
			const VTX_EVENT UPDATE_PROGRESS_BAR = "UPDATE_PROGRESS_BAR";
		} // namespace Global

		// Model events for notifier pattern (model->views)[1-n].
		enum class VTX_EVENT_MODEL : int
		{
			RENDER
		};

		// Event structures.
		struct VTXEvent
		{
			VTXEvent( const VTX_EVENT & p_event ) : name( p_event ) {}
			virtual ~VTXEvent() = default;
			VTX_EVENT name;
		};

		template<typename T>
		struct VTXEventArg : public VTXEvent
		{
			VTXEventArg( const VTX_EVENT & p_event, const T & p_arg ) : VTXEvent( p_event ), arg( p_arg ) {}
			const T arg;
		};

		struct VTXEventFloat : public VTXEventArg<float>
		{
			VTXEventFloat( const VTX_EVENT & p_event, const float p_arg ) : VTXEventArg( p_event, p_arg ) {}
		};

		struct VTXEventString : public VTXEventArg<std::string>
		{
			VTXEventString( const VTX_EVENT & p_event, const std::string & p_arg ) : VTXEventArg( p_event, p_arg ) {}
		};

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
