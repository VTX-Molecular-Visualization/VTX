#ifndef __VTX_ACTION_MOLECULE_CHANGE_IS_PLAYING__
#define __VTX_ACTION_MOLECULE_CHANGE_IS_PLAYING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeIsPlaying : public BaseAction
		{
		  public:
			explicit MoleculeChangeIsPlaying( Model::Molecule & p_molecule, const bool p_isPlaying ) :
				_molecule( p_molecule ), _isPlaying( p_isPlaying )
			{
			}

			virtual void execute() override { _molecule.setIsPlaying( _isPlaying ); }

		  private:
			Model::Molecule & _molecule;
			const bool		  _isPlaying;
		};
	} // namespace Action
} // namespace VTX
#endif
