#ifndef __VTX_APP_EVENT__
#define __VTX_APP_EVENT__

#include "core/event/vtx_event.hpp"
#include "manager/event_manager.hpp"
// #include <memory>
#include <type_traits>

// TODO Apply VTX_ACTION<> whenever it's possible
namespace VTX
{
	// TODO when event manager will be abstracted, this call must be moved in event/vtx_event.hpp

	// TODO when event manager will be abstracted, this call must be moved in event/vtx_event.hpp
	inline void VTX_EVENT( VTX::App::Core::Event::VTXEvent * const p_event )
	{
		VTX::App::Manager::EventManager::get().fireEventVTX( p_event );
	}

	inline void VTX_EVENT( const App::Core::Event::VTX_EVENT & p_eventID )
	{
		App::Core::Event::VTXEvent * const event = new App::Core::Event::VTXEvent( p_eventID );
		VTX::App::Manager::EventManager::get().fireEventVTX( event );
	}

	template<typename... Args>
	inline void VTX_EVENT( const App::Core::Event::VTX_EVENT & p_eventID, Args... p_args )
	{
		App::Core::Event::VTXEventArg<Args...> * const event
			= new App::Core::Event::VTXEventArg<Args...>( p_eventID, p_args... );
		VTX::App::Manager::EventManager::get().fireEventVTX( event );
	}

} // namespace VTX

#endif
