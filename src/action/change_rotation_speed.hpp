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
		class ChangeRotationSpeed : public BaseAction
		{
		  public:
			explicit ChangeRotationSpeed( const float p_speed ) : _speed( p_speed ) {}

			virtual void execute() override
			{
				VTX_SETTING().rotationSpeed = Util::Math::clamp(
					_speed, Setting::CONTROLLER_ROTATION_SPEED_MIN, Setting::CONTROLLER_ROTATION_SPEED_MAX );
			};

		  private:
			const float _speed;
		};
	} // namespace Action
} // namespace VTX
#endif
