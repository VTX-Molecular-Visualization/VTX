#ifndef __VTX_STATE_LOAD__
#define __VTX_STATE_LOAD__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class Load : public BaseState
		{
		  public:
			Load() = default;
			virtual std::string getName() const override { return ID::State::LOAD; }
			virtual void		enter( void * const ) override;
			virtual void		exit() override;

		  private:
			void _loadFile( const IO::Path & ) const;
		};
	} // namespace State
} // namespace VTX
#endif
