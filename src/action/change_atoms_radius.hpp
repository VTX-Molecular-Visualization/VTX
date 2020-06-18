#ifndef __VTX_ACTION_CHANGE_ATOMS_RADIUS__
#define __VTX_ACTION_CHANGE_ATOMS_RADIUS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAtomsRadius : public BaseAction
		{
		  public:
			explicit ChangeAtomsRadius( const float p_atomsRadius ) :
				_atomsRadius( VTX_SETTING().representation == Generic::REPRESENTATION::BALL_AND_STICK
								  ? Util::Math::max( VTX_SETTING().bondsRadius, p_atomsRadius )
								  : p_atomsRadius )
			{
			}

			virtual void execute() override { VTX_SETTING().atomsRadius = _atomsRadius; }

		  private:
			const float _atomsRadius;
		};
	} // namespace Action
} // namespace VTX
#endif
