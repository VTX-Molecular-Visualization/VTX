#ifndef __VTX_STATE_PLAYING__
#define __VTX_STATE_PLAYING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class StatePlaying : public BaseState
		{
		  public:
			StatePlaying() = default;

			virtual std::string getName() const override { return ID::State::PLAYING; }
			virtual void		enter( void * const ) override;
			virtual void		exit() override;
			virtual void		update( const double ) override;

		  private:
			float _time		 = 0.f;
			float _totalTime = 0.f;

			void _setCamera() const;
		};
	} // namespace State
} // namespace VTX
#endif
