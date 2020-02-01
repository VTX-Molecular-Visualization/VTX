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

			virtual std::string getName() const override { return ID::State::VISUALIZATION; }
			virtual void		enter( void * const ) override;
			virtual void		exit() override;
		};
	} // namespace State
} // namespace VTX
#endif
