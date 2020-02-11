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
		class Visualization : public BaseState
		{
		  public:
			Visualization() = default;

			virtual std::string getName() const override { return ID::State::VISUALIZATION; }
			virtual void		enter( void * const ) override;
			virtual void		exit() override;

			virtual void update( const double p_deltaTime ) override;
		};
	} // namespace State
} // namespace VTX
#endif
