#ifndef __VTX_STATE_VISUALIZATION__
#define __VTX_STATE_VISUALIZATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class StateVisualization : public BaseState
		{
		  public:
			StateVisualization() = default;

			virtual STATE_NAME getName() const override { return STATE_NAME::VISUALIZATION; }
			virtual void	   enter() override;
			virtual void	   exit() override;
		};
	} // namespace State
} // namespace VTX
#endif
