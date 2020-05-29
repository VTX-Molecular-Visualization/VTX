#ifndef __VTX_ACTION_ATOM_COLORABLE_CHANGE_COLOR__
#define __VTX_ACTION_ATOM_COLORABLE_CHANGE_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class AtomChangeColor : public BaseAction
		{
		  public:
			explicit AtomChangeColor( Model::Atom & p_atom, const Color::Rgb & p_color ) :
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
	} // namespace Action
} // namespace VTX
#endif
