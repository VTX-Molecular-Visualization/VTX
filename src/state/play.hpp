#ifndef __VTX_STATE_PLAY__
#define __VTX_STATE_PLAY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace State
	{
		class Play : public BaseState
		{
		  public:
			Play() = default;

			virtual const std::string & getName() const override { return ID::State::PLAY; }
			virtual void				enter( void * const ) override;
			virtual void				exit() override;
			virtual void				update( const double ) override;

		  private:
			Model::Path * _path = nullptr;
			float		  _time = 0.f;

			void _setCamera() const;
		};
	} // namespace State
} // namespace VTX
#endif
