#ifndef __VTX_ACTION_MOLECULE_CHANGE_COLOR__
#define __VTX_ACTION_MOLECULE_CHANGE_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeColor : public BaseAction
		{
		  public:
			explicit MoleculeChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) :
				_molecule( p_molecule ), _color( p_color )
			{
			}

			virtual void execute() override
			{
				_molecule.setColor( _color );
				_molecule.setColorMode();
			}

		  private:
			Model::Molecule & _molecule;
			const Color::Rgb  _color;
		};
	} // namespace Action
} // namespace VTX
#endif
