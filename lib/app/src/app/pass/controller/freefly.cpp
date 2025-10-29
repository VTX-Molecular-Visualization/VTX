#include "app/pass/controller/freefly.hpp"
#include "app/input/input_manager.hpp"
#include "app/input/key_MAPPING.hpp"
#include "app/services.hpp"
#include <util/constants.hpp>
#include <util/math/transform.hpp>

namespace
{
	using namespace VTX::App;

	enum Keys
	{
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_FRONT,
		MOVE_BACK,
		MOVE_UP,
		MOVE_DOWN,
	};

	auto _MAPPING = Input::KeyMapping(
		{
			{ Keys::MOVE_LEFT, { Input::Key::Key_Left, Input::InputManager::getKeyFromQwerty( Input::Key::Key_A ) } },
			{ Keys::MOVE_RIGHT, { Input::Key::Key_Right, Input::InputManager::getKeyFromQwerty( Input::Key::Key_D ) } },
			{ Keys::MOVE_FRONT, { Input::Key::Key_Up, Input::InputManager::getKeyFromQwerty( Input::Key::Key_W ) } },
			{ Keys::MOVE_BACK, { Input::Key::Key_Down, Input::InputManager::getKeyFromQwerty( Input::Key::Key_S ) } },
			{ Keys::MOVE_UP, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_R ) } },
			{ Keys::MOVE_DOWN, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_F ) } },
		}
	);

} // namespace

namespace VTX::App::Pass::Controller
{

	void Freefly::update( const float p_deltaTime, const float p_elapsedTime )
	{
		using namespace Util;
		auto & input = INPUT();

		//  Rotation.
		Vec2i deltaVelocityInput = input.consumeDeltaMousePosition();

		Vec3f localRotation = VEC3F_ZERO;
		if ( input.isMouseLeftPressed() )
		{
			localRotation = Vec3f(
				-rotationSpeed * deltaVelocityInput.y * ( invertY ? -1.f : 1.f ),
				-rotationSpeed * deltaVelocityInput.x,
				0.f
			);
		}
		float rollRotation = 0.f;
		if ( input.isMouseRightPressed() )
		{
			rollRotation = -rotationSpeed * deltaVelocityInput.x;
		}

		// Translation.
		Vec3f translation = VEC3F_ZERO;
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_FRONT ] ) )
		{
			translation.z--;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_BACK ] ) )
		{
			translation.z++;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_LEFT ] ) )
		{
			translation.x--;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_RIGHT ] ) )
		{
			translation.x++;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_UP ] ) )
		{
			translation.y++;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_DOWN ] ) )
		{
			translation.y--;
		}

		if ( translation != VEC3F_ZERO )
		{
			translation *= translationSpeed;
			translation *= p_deltaTime * 1e-3f;

			if ( input.isModifierExclusive( Input::Modifier::Shift ) )
			{
				translation *= accelerationFactor;
			}
			if ( input.isModifierExclusive( Input::Modifier::Alt ) )
			{
				translation /= decelerationFactor;
			}
		}

		if ( localRotation != VEC3F_ZERO || rollRotation != 0.f || translation != VEC3F_ZERO )
		{
			REG().patch<Math::Transform>(
				_cameraEntity,
				[ &localRotation, rollRotation, &translation ]( Math::Transform & p_transform )
				{
					if ( localRotation != VEC3F_ZERO )
						p_transform.rotate( localRotation );
					if ( rollRotation != 0.f )
						p_transform.rotateRoll( rollRotation );
					if ( translation != VEC3F_ZERO )
						p_transform.translate( translation );
				}
			);
		}
		//_camera->getTransform().localMove( translation );
	}

} // namespace VTX::App::Pass::Controller
