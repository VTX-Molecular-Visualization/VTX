#ifndef __VTX_CONTROLLER_PICKER__
#define __VTX_CONTROLLER_PICKER__

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"

namespace VTX
{
	namespace Model
	{
		class Atom;
		class Residue;
	} // namespace Model

	namespace Controller
	{
		class Picker : public BaseMouseController, public BaseKeyboardController
		{
		  public:
			void update( const float & p_deltaTime ) override;

		  protected:
			void _onMouseLeftClick( const uint p_x, const uint p_y ) override;

			void _selectItem( Model::Atom & p_atomPicked ) const;
			void _selectItem( Model::Atom & p_atomPicked1, Model::Atom & p_atomPicked2 ) const;
			void _selectItem( Model::Residue & p_residuePicked ) const;

			void _unselectItem( Model::Atom & p_atomPicked ) const;
			void _unselectItem( Model::Atom & p_atomPicked1, Model::Atom & p_atomPicked2 ) const;
			void _unselectItem( Model::Residue & p_residuePicked ) const;
		};
	} // namespace Controller

} // namespace VTX

#endif
