#include "app/controller/camera/freefly.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"

namespace VTX::App::Controller::Camera
{
	Freefly::Freefly()
	{
		using namespace App::Core;

		// TODO: ??
		_mapping = Input::KeyMapping(
			{
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
			}
		);
	}

	void Freefly::update( const float p_deltaTime, const float p_elapsedTime )
	{
		using namespace App::Core;

		//  Rotation.
		if ( INPUT().isMouseLeftPressed() )
		{
			_camera->getTransform().localRotate( Vec3f(
				-rotationSpeed * INPUT().getDeltaMousePosition().y * ( invertY ? -1.f : 1.f ),
				-rotationSpeed * INPUT().getDeltaMousePosition().x,
				0.f
			) );
		}
		if ( INPUT().isMouseRightPressed() )
		{
			_camera->getTransform().rotateRoll( rotationSpeed * INPUT().getDeltaMousePosition().x );
		}

		// Translation.
		Vec3f translation = VEC3F_ZERO;

		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_FRONT ) ] ) )
		{
			translation.z++;
		}
		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_BACK ) ] ) )
		{
			translation.z--;
		}
		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_LEFT ) ] ) )
		{
			translation.x--;
		}
		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_RIGHT ) ] ) )
		{
			translation.x++;
		}
		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_UP ) ] ) )
		{
			translation.y++;
		}
		if ( INPUT().isAnyKeyPressed( _mapping[ int( Keys::MOVE_DOWN ) ] ) )
		{
			translation.y--;
		}

		if ( translation == VEC3F_ZERO )
		{
			return;
		}

		translation *= translationSpeed;
		translation *= p_deltaTime * 1e-3f;

		if ( INPUT().isModifierExclusive( Input::Modifier::Shift ) )
		{
			translation *= accelerationFactor;
		}
		if ( INPUT().isModifierExclusive( Input::Modifier::Alt ) )
		{
			translation /= decelerationFactor;
		}

		_camera->getTransform().localMove( translation );
	}

} // namespace VTX::App::Controller::Camera
