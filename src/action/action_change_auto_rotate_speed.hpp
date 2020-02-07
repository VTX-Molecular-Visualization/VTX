#ifndef __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__
#define __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeAutoRotateSpeed : public BaseAction
		{
		  public:
			explicit ActionChangeAutoRotateSpeed( const Vec3f & p_value ) : _value( p_value ) {}

			virtual void execute() override
			{
				Setting::Controller::autoRotateSpeed
					= glm::clamp( _value, AUTO_ROTATE_SPEED_MIN, AUTO_ROTATE_SPEED_MAX );
			};

		  private:
			const Vec3f & _value;
		};
	} // namespace Action
} // namespace VTX
#endif
