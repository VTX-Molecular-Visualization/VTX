#ifndef __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__
#define __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeAutoRotateSpeed : public BaseAction
		{
		  public:
			explicit ActionChangeAutoRotateSpeed( const Vec3f & p_value ) : _value( p_value ) {}

			virtual void execute() override { Setting::Controller::autoRotateSpeed = _value; };

		  private:
			const Vec3f & _value;
		};
	} // namespace Action
} // namespace VTX
#endif
