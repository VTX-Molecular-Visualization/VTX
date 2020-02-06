#ifndef __VTX_ACTION_CHANGE_TRANSLATION_FACTOR_SPEED__
#define __VTX_ACTION_CHANGE_TRANSLATION_FACTOR_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeTranslationFactorSpeed : public BaseAction
		{
		  public:
			explicit ActionChangeTranslationFactorSpeed( const float p_factor ) : _factor( p_factor ) {}

			virtual void execute() override { Setting::Controller::translationFactorSpeed = _factor; };

		  private:
			const float _factor;
		};
	} // namespace Action
} // namespace VTX
#endif
