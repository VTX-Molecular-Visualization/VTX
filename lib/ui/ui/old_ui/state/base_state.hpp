#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#include "controller/base_controller.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include <map>

namespace VTX
{
	namespace State
	{
		class BaseState : public Generic::BaseUpdatable, public Event::BaseEventReceiverVTX
		{
		  public:
			BaseState() = default;
			virtual ~BaseState()
			{
				for ( const std::pair<const ID::VTX_ID, Controller::BaseController * const> & pair : _controllers )
				{
					delete pair.second;
				}
				_controllers.clear();
			}

			template<typename T, typename = std::enable_if<std::is_base_of<Controller::BaseController, T>::value>>
			inline T * const getController( const ID::VTX_ID & p_id )
			{
				return dynamic_cast<T * const>( _controllers[ p_id ] );
			}
			template<typename T, typename = std::enable_if<std::is_base_of<Controller::BaseController, T>::value>>
			inline const T * const getController( const ID::VTX_ID & p_id ) const
			{
				return dynamic_cast<T * const>( _controllers.at( p_id ) );
			}

			virtual void enter( void * const )
			{
				for ( const std::pair<const ID::VTX_ID, Controller::BaseController * const> & pair : _controllers )
				{
					pair.second->setActive( true );
				}
			}

			virtual void exit()
			{
				for ( const std::pair<const ID::VTX_ID, Controller::BaseController * const> & pair : _controllers )
				{
					pair.second->setActive( false );
				}
			}

			virtual void update( const float & p_deltaTime ) override
			{
				for ( const std::pair<const ID::VTX_ID, Controller::BaseController * const> & pair : _controllers )
				{
					if ( pair.second->isActive() )
					{
						pair.second->update( p_deltaTime );
					}
				}
			}

		  protected:
			std::map<const ID::VTX_ID, Controller::BaseController * const> _controllers
				= std::map<const ID::VTX_ID, Controller::BaseController * const>();

		}; // namespace State
	}	   // namespace State
} // namespace VTX
#endif
