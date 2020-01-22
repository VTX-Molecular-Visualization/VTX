#ifndef __VTX_STATE_LOADING__
#define __VTX_STATE_LOADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"
#include <string>

namespace VTX
{
	namespace State
	{
		class StateLoading : public BaseState
		{
		  public:
			StateLoading() = default;

			virtual STATE_NAME getStateName() const override { return STATE_NAME::LOADING; }
			virtual void	   enter( void * const ) override;
			virtual void	   exit() override;

		  private:
			void _loadFile( std::string * ) const;
		};
	} // namespace State
} // namespace VTX
#endif
