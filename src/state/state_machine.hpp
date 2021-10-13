#ifndef __VTX_STATE_MACHINE__
#define __VTX_STATE_MACHINE__

#include "base_state.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class StateMachine : public Generic::BaseUpdatable
		{
		  public:
			StateMachine();
			~StateMachine();

			template<typename T>
			inline T * const getState( const ID::VTX_ID & p_id )
			{
				return static_cast<T * const>( _states[ p_id ] );
			}

			void goToState( const ID::VTX_ID &, void * const p_arg = nullptr );

			virtual void update( const float & ) override;

		  private:
			BaseState *									  _currentState = nullptr;
			std::map<const ID::VTX_ID, BaseState * const> _states = std::map<const ID::VTX_ID, BaseState * const>();

			void _switchState( BaseState * const, void * const p_arg );
		};
	}; // namespace State
};	   // namespace VTX
#endif
