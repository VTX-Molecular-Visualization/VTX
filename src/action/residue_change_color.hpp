#ifndef __VTX_ACTION_RESIDUE_CHANGE_COLOR__
#define __VTX_ACTION_RESIDUE_CHANGE_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ResidueChangeColor : public BaseAction
		{
		  public:
			explicit ResidueChangeColor( Model::Residue & p_residue, const Color::Rgb & p_color ) :
				_residue( p_residue ), _color( p_color )
			{
			}

			virtual void execute() override
			{
				_residue.setColor( _color );
				_residue.getMoleculePtr()->setColorMode();
			}

		  private:
			Model::Residue & _residue;
			const Color::Rgb _color;
		};
	} // namespace Action
} // namespace VTX
#endif
