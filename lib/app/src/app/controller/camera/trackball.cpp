#include "app/controller/camera/trackball.hpp"
#include "app/application/scene.hpp"
#include "app/core/input/input_manager.hpp"
#include <util/fmt/glm.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Camera
{
	Trackball::Trackball()
	{
		using namespace App::Core;

		_mapping = Input::KeyMapping( {
			{ int( Keys::MOVE_FRONT ),
			  { Input::Key::Key_Up, Input::InputManager::getKeyFromQwerty( Input::Key::Key_W ) } },
			{ int( Keys::MOVE_BACK ),
			  { Input::Key::Key_Down, Input::InputManager::getKeyFromQwerty( Input::Key::Key_S ) } },
			{ int( Keys::ROTATE_LEFT ),
			  { Input::Key::Key_Left, Input::InputManager::getKeyFromQwerty( Input::Key::Key_A ) } },
			{ int( Keys::ROTATE_RIGHT ),
			  { Input::Key::Key_Right, Input::InputManager::getKeyFromQwerty( Input::Key::Key_D ) } },
			{ int( Keys::ROTATE_UP ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_R ) } },
			{ int( Keys::ROTATE_DOWN ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_F ) } },
			{ int( Keys::ROLL_LEFT ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_Q ) } },
			{ int( Keys::ROLL_RIGHT ), { Input::InputManager::getKeyFromQwerty( Input::Key::Key_E ) } },
		} );
	}

	/*

	Vec3f Trackball::targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const
	{
		const float distanceToTarget
			= VTX::Util::Math::distance( p_camera.getTransform().getPosition(), p_camera.getTarget() );

		return p_camera.getTransform().getPosition() + p_camera.getTransform().getFront() * distanceToTarget;
	}
	*/

	void Trackball::update( const float p_deltaTime, const float p_elapsedTime )
	{
		float deltaTime = p_deltaTime * 1e-3f;
		using namespace App::Core;

		// Wheel.
		float deltaDistance = 0.f;
		if ( INPUT_MANAGER().getDeltaMouseWheel() != 0 )
		{
			deltaDistance = INPUT_MANAGER().getDeltaMouseWheel() * 0.00001f
							* VTX::Util::Math::distance( _camera->getTransform().getPosition(), _camera->getTarget() );
		}

		// Mouse left.
		Vec3f deltaVelocity = VEC3F_ZERO;
		if ( INPUT_MANAGER().isMouseLeftPressed() )
		{
			deltaVelocity.x = -INPUT_MANAGER().getDeltaMousePosition().x * 15.f;
			deltaVelocity.y = -INPUT_MANAGER().getDeltaMousePosition().y * 15.f;
		}
		// Mouse right.
		else if ( INPUT_MANAGER().isMouseRightPressed() )
		{
			deltaVelocity.z = INPUT_MANAGER().getDeltaMousePosition().x * 15.f;
		}
		// Pan target with wheel button.
		else if ( INPUT_MANAGER().isMouseMiddlePressed() )
		{
			float deltaX = -INPUT_MANAGER().getDeltaMousePosition().x * 0.1f;
			float deltaY = INPUT_MANAGER().getDeltaMousePosition().y * 0.1f;

			const Vec3f newTarget = _camera->getTarget()
									+ _camera->getTransform().getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );

			_camera->setTargetWorld( newTarget );

			_needUpdate = true;
		}

		// Keyboard.
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_FRONT ) ] ) )
		{
			deltaDistance = 1.5f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_BACK ) ] ) )
		{
			deltaDistance = -1.5f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_RIGHT ) ] ) )
		{
			deltaVelocity.x = 1e4f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_LEFT ) ] ) )
		{
			deltaVelocity.x = -1e4f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_UP ) ] ) )
		{
			deltaVelocity.y = 1e4f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_DOWN ) ] ) )
		{
			deltaVelocity.y = -1e4f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROLL_RIGHT ) ] ) )
		{
			deltaVelocity.z = 1e4f * deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROLL_LEFT ) ] ) )
		{
			deltaVelocity.z = -1e4f * deltaTime;
		}

		// Set values from settings.
		if ( deltaDistance != 0.f )
		{
			deltaDistance *= translationSpeed;

			if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Shift ) )
			{
				deltaDistance *= accelerationFactor;
			}
			if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Alt ) )
			{
				deltaDistance /= decelerationFactor;
			}

			_needUpdate = true;
		}

		if ( deltaVelocity != VEC3F_ZERO )
		{
			if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Shift ) )
			{
				deltaVelocity *= accelerationFactor;
			}
			if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Alt ) )
			{
				deltaVelocity /= decelerationFactor;
			}

			_velocity.x += rotationSpeed * deltaVelocity.x;
			_velocity.y += rotationSpeed * deltaVelocity.y * ( invertY ? -1.f : 1.f );
			_velocity.z += rotationSpeed * deltaVelocity.z;
		}

		_needUpdate |= _velocity != VEC3F_ZERO;

		// Update if needed.
		if ( _needUpdate )
		{
			float distance = VTX::Util::Math::distance( _camera->getTransform().getPosition(), _camera->getTarget() );
			distance	   = VTX::Util::Math::clamp( distance - deltaDistance, 0.1f, 10000.f );

			const Quatf rotation
				= Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z ) * ( elasticityActive ? deltaTime : 0.2f ) );

			_camera->getTransform().rotateAround( rotation, _camera->getTarget(), distance );
			// float d = Util::distance( _camera.getPosition(), _target );
			// VTX_LOG_FILE( std::to_string( deltaTime ) + " / " + std::to_string( distance ) + " / "
			//			  + std::to_string( d ) );
			_needUpdate = false;
		}

		// Handle elasticity.
		if ( elasticityActive )
		{
			_updateElasticity( deltaTime );
		}
		else
		{
			_velocity = VEC3F_ZERO;
		}
	}

	void Trackball::_updateElasticity( const float & p_deltaTime )
	{
		if ( _velocity != VEC3F_ZERO )
		{
			_velocity = VTX::Util::Math::linearInterpolation( _velocity, VEC3F_ZERO, p_deltaTime * elasticityFactor );

			Vec3f::bool_type res = VTX::Util::Math::lessThan(
				VTX::Util::Math::abs( _velocity ), Vec3f( _CONTROLLER_ELASTICITY_THRESHOLD )
			);

			if ( not INPUT_MANAGER().isMouseLeftPressed() && res.x && res.y && res.z )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}

} // namespace VTX::App::Controller::Camera
