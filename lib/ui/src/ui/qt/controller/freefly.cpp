#include "ui/qt/controller/freefly.hpp"
#include "ui/internal/all_settings.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include <app/application/settings.hpp>

namespace VTX::UI::QT::Controller
{
	void Freefly::init()
	{
		_mapping = Input::KeyMapping( {
			{ int( Keys::MOVE_LEFT ),
			  { Input::Key::Key_Left, Input::InputManager::getKeyFromQwerty( Input::Key::Key_A ) } },
			{ int( Keys::MOVE_RIGHT ),
			  { Input::Key::Key_Right, Input::InputManager::getKeyFromQwerty( Input::Key::Key_D ) } },
			{ int( Keys::MOVE_FRONT ),
			  { Input::Key::Key_Up, Input::InputManager::getKeyFromQwerty( Input::Key::Key_W ) } },
			{ int( Keys::MOVE_BACK ),
			  { Input::Key::Key_Down, Input::InputManager::getKeyFromQwerty( Input::Key::Key_S ) } },
			{ int( Keys::MOVE_UP ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_R ) } },
			{ int( Keys::MOVE_DOWN ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_F ) } },
		} );
	}
	void Freefly::_updateInputs( const float & p_deltaTime )
	{
		if ( !QT_APP()->getMainWindow().getRender()->hasFocus() )
			return;

		// Rotation.
		if ( INPUT_MANAGER().isMouseLeftPressed() )
		{
			getCamera().rotate( Vec3f(
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().y * ( invertY ? -1.f : 1.f ),
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x,
				0.f
			) );
		}
		if ( INPUT_MANAGER().isMouseRightPressed() )
		{
			getCamera().rotateRoll( rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x );
		}

		// Translation.
		Vec3f translation = VEC3F_ZERO;

		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_FRONT ) ] ) )
		{
			translation.z++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_BACK ) ] ) )
		{
			translation.z--;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_LEFT ) ] ) )
		{
			translation.x--;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_RIGHT ) ] ) )
		{
			translation.x++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_UP ) ] ) )
		{
			translation.y++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_DOWN ) ] ) )
		{
			translation.y--;
		}

		if ( translation == VEC3F_ZERO )
		{
			return;
		}

		translation *= translationSpeed;
		translation *= p_deltaTime;

		if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Shift ) )
		{
			translation *= accelerationFactor;
		}
		if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Alt ) )
		{
			translation /= decelerationFactor;
		}

		getCamera().move( translation );
	}

} // namespace VTX::UI::QT::Controller
