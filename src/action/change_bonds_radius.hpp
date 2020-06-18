#ifndef __VTX_ACTION_CHANGE_BONDS_RADIUS__
#define __VTX_ACTION_CHANGE_BONDS_RADIUS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeBondsRadius : public BaseAction
		{
		  public:
			explicit ChangeBondsRadius( const float p_bondsRadius ) :
				_bondsRadius( VTX_SETTING().representation == Generic::REPRESENTATION::BALL_AND_STICK
								  ? Util::Math::min( VTX_SETTING().atomsRadius, p_bondsRadius )
								  : p_bondsRadius )
			{
			}

			virtual void execute() override { VTX_SETTING().bondsRadius = _bondsRadius; };

		  private:
			const float _bondsRadius;
		};
	} // namespace Action
} // namespace VTX
#endif
