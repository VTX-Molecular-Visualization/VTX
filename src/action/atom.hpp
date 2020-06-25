#ifndef __VTX_ACTION_ATOM__
#define __VTX_ACTION_ATOM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Atom
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Atom & p_atom, const Color::Rgb & p_color ) :
					_atom( p_atom ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_atom.setColor( _color );
					_atom.getMoleculePtr()->setColorMode();
				}

			  private:
				Model::Atom &	 _atom;
				const Color::Rgb _color;
			};
		} // namespace Atom
	} // namespace Action
} // namespace VTX
#endif
