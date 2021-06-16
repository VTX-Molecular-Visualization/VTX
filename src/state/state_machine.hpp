#ifndef __VTX_STATE_MACHINE__
#define __VTX_STATE_MACHINE__

#include "base_state.hpp"
#include "generic/base_updatable.hpp"
#include "generic/has_collection.hpp"
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class StateMachine : public Generic::BaseUpdatable, public Generic::HasCollection<BaseState>
		{
		  public:
			StateMachine();

			void goToState( const std::string &, void * const p_arg = nullptr );

			virtual void update( const float & ) override;

		  private:
			BaseState * _currentState = nullptr;

			void _switchState( BaseState * const, void * const p_arg );
		};
	}; // namespace State
};	   // namespace VTX
#endif
