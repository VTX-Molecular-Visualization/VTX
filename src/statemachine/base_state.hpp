#include "state_machine.hpp"

namespace VTX
{
	namespace StateMachine
	{
		class BaseState : public StateMachine
		{
		  protected:
			virtual void _enter() = 0;
			virtual void _exit()  = 0;
		};
	} // namespace StateMachine
} // namespace VTX
