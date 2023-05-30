#ifndef __VTX_CONTROLLER_PICKER__
#define __VTX_CONTROLLER_PICKER__

#include "app/component/chemistry/_fwd.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "ui/id.hpp"

namespace VTX::Controller
{
	class Picker : public BaseMouseController, public BaseKeyboardController
	{
	  public:
		Picker();

		inline App::VTX_ID getTargetWidget() override { return UI::ID::Input::RENDER_WIDGET; }
		void			   update( const float & p_deltaTime ) override;

	  protected:
		void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
		void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) override;
		void _onMouseRightClick( const uint p_x, const uint p_y ) override;

		void _performSelection( const Vec2i & p_ids ) const;
		bool _isTargetSelected( const Vec2i & p_ids ) const;

		void _selectItem( App::Component::Chemistry::Atom & p_atomPicked ) const;
		void _selectItem( App::Component::Chemistry::Atom & p_atomPicked1,
						  App::Component::Chemistry::Atom & p_atomPicked2 ) const;
		void _selectItem( App::Component::Chemistry::Residue & p_residuePicked ) const;

		void _unselectItem( App::Component::Chemistry::Atom & p_atomPicked ) const;
		void _unselectItem( App::Component::Chemistry::Atom & p_atomPicked1,
							App::Component::Chemistry::Atom & p_atomPicked2 ) const;
		void _unselectItem( App::Component::Chemistry::Residue & p_residuePicked ) const;

	  private:
		Vec2i _lastClickedIds;
	};

} // namespace VTX::Controller
#endif
