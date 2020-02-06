#ifndef __VTX_ACTION_CHANGE_ROTATION_SPEED__
#define __VTX_ACTION_CHANGE_ROTATION_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeRotationSpeed : public BaseAction
		{
		  public:
			explicit ActionChangeRotationSpeed( const float p_speed ) : _speed( p_speed ) {}

			virtual void execute() override { Setting::Controller::rotationSpeed = _speed; };

		  private:
			const float _speed;
		};
	} // namespace Action
} // namespace VTX
#endif
