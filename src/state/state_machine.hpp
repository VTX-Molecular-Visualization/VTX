#ifndef __VTX_STATE_MACHINE__
#define __VTX_STATE_MACHINE__

#include "../lib/magic_enum.hpp"
#include "base_state.hpp"
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class StateMachine
		{
		  public:
			virtual ~StateMachine();
			virtual void init();

		  protected:
			virtual void _addState( const std::shared_ptr<BaseState> ) final;
			virtual void _addStates() {};

		  private:
			std::map<STATE_NAME, std::shared_ptr<BaseState>> _states
				= std::map<STATE_NAME, std::shared_ptr<BaseState>>();
		};
	}; // namespace State
};	   // namespace VTX
#endif
