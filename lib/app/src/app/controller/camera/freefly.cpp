#include "app/controller/camera/freefly.hpp"
#include "app/core/input/input_manager.hpp"

namespace VTX::App::Controller::Camera
{
	void Freefly::init()
	{
		using namespace App::Core;

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

	void Freefly::setActive( const bool p_active )
	{
		BaseCameraController::setActive( p_active );

		if ( p_active )
			getCamera().attachTarget();
		else
			getCamera().detachTarget();
	}

	void Freefly::_updateInputs( const float & p_deltaTime )
	{
		using namespace App::Core;

		// if ( !QT_APP()->getMainWindow().getRender()->hasFocus() )
		//	return;

		// Rotation.
		if ( INPUT_MANAGER().isMouseLeftPressed() )
		{
			getCamera().getTransform().localRotate( Vec3f(
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().y * ( invertY ? -1.f : 1.f ),
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x,
				0.f
			) );
		}
		if ( INPUT_MANAGER().isMouseRightPressed() )
		{
			getCamera().getTransform().rotateRoll( rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x );
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

		getCamera().getTransform().localMove( translation );
	}

} // namespace VTX::App::Controller::Camera
