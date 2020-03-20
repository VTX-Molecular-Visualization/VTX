#ifndef __VTX_ACTION_MOLECULE_CHANGE_FRAME__
#define __VTX_ACTION_MOLECULE_CHANGE_FRAME__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeFrame : public BaseAction
		{
		  public:
			explicit MoleculeChangeFrame( Model::Molecule & p_molecule, const int p_frame ) :
				_molecule( p_molecule ), _frame( p_frame )
			{
			}

			virtual void execute() override
			{
				_molecule.setFrame( Util::Math::clamp( _frame, 0, (int)_molecule.getFrameCount() - 1 ) );
			}

		  private:
			Model::Molecule & _molecule;
			const int		  _frame;
		};
	} // namespace Action
} // namespace VTX
#endif
