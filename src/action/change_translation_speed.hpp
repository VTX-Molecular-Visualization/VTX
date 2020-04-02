#ifndef __VTX_ACTION_CHANGE_TRANSLATION_SPEED__
#define __VTX_ACTION_CHANGE_TRANSLATION_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeTranslationSpeed : public BaseAction
		{
		  public:
			explicit ChangeTranslationSpeed( const float p_speed ) : _speed( p_speed ) {}

			virtual void execute() override
			{
				Setting::Controller::translationSpeed
					= Util::Math::clamp( _speed, CONTROLLER_TRANSLATION_SPEED_MIN, CONTROLLER_TRANSLATION_SPEED_MAX );
			};

		  private:
			const float _speed;
		};
	} // namespace Action
} // namespace VTX
#endif
