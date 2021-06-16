#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#include "controller/base_controller.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_updatable.hpp"
#include "generic/has_collection.hpp"
#include "id.hpp"
#include <vector>

namespace VTX
{
	namespace State
	{
		class BaseState : public Generic::HasCollection<Controller::BaseController>, public Generic::BaseUpdatable, public Event::BaseEventReceiverVTX
		{
		  public:
			BaseState()			 = default;
			virtual ~BaseState() = default;

			virtual void enter( void * const ) = 0;
			virtual void exit()				   = 0;

			virtual void BaseState::update( const float & p_deltaTime ) override
			{
				for ( const PairStringToItemPtr & controller : _items )
				{
					if ( controller.second->isActive() )
					{
						controller.second->update( p_deltaTime );
					}
				}
			}
		}; // namespace State
	}	   // namespace State
} // namespace VTX
#endif
