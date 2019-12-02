#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

namespace VTX
{
	namespace State
	{
		enum class STATE_NAME
		{
		};

		class StateMachine;
		class BaseState : public StateMachine
		{
		  public:
			virtual STATE_NAME getName() const = 0;

		  protected:
			virtual void _enter() = 0;
			virtual void _exit()  = 0;
		};
	} // namespace State
} // namespace VTX
#endif
