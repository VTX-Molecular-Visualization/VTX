#ifndef __VTX_STATE_LOADING__
#define __VTX_STATE_LOADING__

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class StateLoading : public BaseState
		{
		  public:
			StateLoading() = default;

			virtual STATE_NAME getName() const override { return STATE_NAME::LOADING; }
			virtual void	   enter() override;
			virtual void	   exit() override;
		};
	} // namespace State
} // namespace VTX
#endif
