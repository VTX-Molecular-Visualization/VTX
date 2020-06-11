#ifndef __VTX_ACTION_CHANGE_ATOMS_BONDS_RADIUS__
#define __VTX_ACTION_CHANGE_ATOMS_BONDS_RADIUS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAtomsBondsRadius : public BaseAction
		{
		  public:
			explicit ChangeAtomsBondsRadius( const float p_atomsRadius, const float p_bondsRadius ) :
				_atomsRadius( p_atomsRadius ), _bondsRadius( p_bondsRadius )
			{
			}

			virtual void execute() override
			{
				Setting::MoleculeView::atomsRadiusFixed = Util::Math::max( _atomsRadius, _bondsRadius );
				Setting::MoleculeView::bondsRadius		= Util::Math::min( _atomsRadius, _bondsRadius );
			};

		  private:
			const float _atomsRadius;
			const float _bondsRadius;
		};
	} // namespace Action
} // namespace VTX
#endif
