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

			template<typename T, typename = std::enable_if<std::is_base_of<BaseState, T>::value>>
			inline T * const getState( const ID::VTX_ID & p_id )
			{
				return dynamic_cast<T * const>( _states[ p_id ] );
			}
			template<typename T, typename = std::enable_if<std::is_base_of<BaseState, T>::value>>
			inline const T * const getState( const ID::VTX_ID & p_id ) const
			{
				return dynamic_cast<const T * const>( _states.at( p_id ) );
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
