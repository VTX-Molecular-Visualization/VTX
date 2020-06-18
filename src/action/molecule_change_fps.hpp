#ifndef __VTX_ACTION_MOLECULE_CHANGE_FPS__
#define __VTX_ACTION_MOLECULE_CHANGE_FPS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeFPS : public BaseAction
		{
		  public:
			explicit MoleculeChangeFPS( Model::Molecule & p_molecule, const int p_fps ) :
				_molecule( p_molecule ), _fps( p_fps )
			{
			}

			virtual void execute() override
			{
				_molecule.setFPS( Util::Math::clamp( _fps, 0, int( Setting::VIDEO_FPS_DEFAULT ) ) );
			}

		  private:
			Model::Molecule & _molecule;
			const int		  _fps;
		};
	} // namespace Action
} // namespace VTX
#endif
