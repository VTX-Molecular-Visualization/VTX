#ifndef __VTX_STATE_LOADING__
#define __VTX_STATE_LOADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class StateLoading : public BaseState
		{
		  public:
			StateLoading() = default;

			virtual std::string getName() const override { return ID::State::LOADING; }
			virtual void		enter( void * const ) override;
			virtual void		exit() override;

		  private:
			void _loadFile( std::string * ) const;
		};
	} // namespace State
} // namespace VTX
#endif
