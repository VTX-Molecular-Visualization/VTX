#ifndef __VTX_UI_QT_CONTROLLER_PICKER__
#define __VTX_UI_QT_CONTROLLER_PICKER__

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "ui/id.hpp"
#include <app/old/component/chemistry/_fwd.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class Picker : public BaseMouseController, public BaseKeyboardController
	{
	  public:
		Picker();

		inline App::Old::VTX_ID getTargetWidget() override { return UI::ID::Input::RENDER_WIDGET; }
		void			   update( const float & p_deltaTime ) override;

	  protected:
		void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
		void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) override;
		void _onMouseRightClick( const uint p_x, const uint p_y ) override;

		void _performSelection( const Vec2i & p_ids ) const;
		bool _isTargetSelected( const Vec2i & p_ids ) const;

		void _selectItem( App::Old::Component::Chemistry::Atom & p_atomPicked ) const;
		void _selectItem( App::Old::Component::Chemistry::Atom & p_atomPicked1,
						  App::Old::Component::Chemistry::Atom & p_atomPicked2 ) const;
		void _selectItem( App::Old::Component::Chemistry::Residue & p_residuePicked ) const;

		void _unselectItem( App::Old::Component::Chemistry::Atom & p_atomPicked ) const;
		void _unselectItem( App::Old::Component::Chemistry::Atom & p_atomPicked1,
							App::Old::Component::Chemistry::Atom & p_atomPicked2 ) const;
		void _unselectItem( App::Old::Component::Chemistry::Residue & p_residuePicked ) const;

	  private:
		Vec2i _lastClickedIds;
	};
} // namespace VTX::UI::QT::Controller

#endif
