#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include <app/application/scene.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	void Trackball::init()
	{
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
	void Trackball::setActive( const bool p_active )
	{
		BaseController::setActive( p_active );

		if ( p_active )
		{
			_target = targetSimulationFromCamera( getCamera() );
		}
		else
		{
			_velocity = VEC3F_ZERO;
			// Save distance to force at next setActive(true).
			// If orient is called in Freefly, the distance is overriden.
			_distanceForced = VTX::Util::Math::distance( getCamera().getTransform().getPosition(), _target );
		}
	}

	Vec3f Trackball::targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const
	{
		return p_camera.getTransform().getPosition() + p_camera.getTransform().getFront() * _distanceForced;
	}

	void Trackball::_updateInputs( const float & p_deltaTime )
	{
		if ( !QT_APP()->getMainWindow().getRender()->hasFocus() )
			return;

		// Wheel.
		float deltaDistance = 0.f;
		if ( INPUT_MANAGER().getDeltaMouseWheel() != 0 )
		{
			deltaDistance = INPUT_MANAGER().getDeltaMouseWheel() * 0.00001
							* VTX::Util::Math::distance( getCamera().getTransform().getPosition(), _target );
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
			float deltaX = -INPUT_MANAGER().getDeltaMousePosition().x * 0.1;
			float deltaY = INPUT_MANAGER().getDeltaMousePosition().y * 0.1;

			_target		= _target + getCamera().getTransform().getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
			_needUpdate = true;
		}

		// Keyboard.
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_FRONT ) ] ) )
		{
			deltaDistance = 1.5f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_BACK ) ] ) )
		{
			deltaDistance = -1.5f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_RIGHT ) ] ) )
		{
			deltaVelocity.x = 1e4f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_LEFT ) ] ) )
		{
			deltaVelocity.x = -1e4f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_UP ) ] ) )
		{
			deltaVelocity.y = 1e4f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROTATE_DOWN ) ] ) )
		{
			deltaVelocity.y = -1e4f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROLL_RIGHT ) ] ) )
		{
			deltaVelocity.z = 1e4f * p_deltaTime;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::ROLL_LEFT ) ] ) )
		{
			deltaVelocity.z = -1e4f * p_deltaTime;
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
			float distance = 0.f;
			if ( _distanceForced != 0.f )
			{
				distance		= VTX::Util::Math::clamp( _distanceForced - deltaDistance, 0.1f, 10000.f );
				_distanceForced = 0.f;
			}
			else
			{
				distance = VTX::Util::Math::distance( getCamera().getTransform().getPosition(), _target );
				distance = VTX::Util::Math::clamp( distance - deltaDistance, 0.1f, 10000.f );
			}

			const Quatf rotation
				= Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z ) * ( elasticityActive ? p_deltaTime : 0.2f ) );
			getCamera().getTransform().rotateAround( rotation, _target, distance );
			// float d = Util::distance( _camera.getPosition(), _target );
			// VTX_LOG_FILE( std::to_string( p_deltaTime ) + " / " + std::to_string( distance ) + " / "
			//			  + std::to_string( d ) );
			_needUpdate = false;
		}

		// Handle elasticity.
		if ( elasticityActive )
		{
			_updateElasticity( p_deltaTime );
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

			if ( !INPUT_MANAGER().isMouseLeftPressed() && res.x && res.y && res.z )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}

	void Trackball::reset()
	{
		BaseCameraController::reset();

		_needUpdate = true;
		_target		= App::SCENE().getAABB().centroid();
		_velocity	= VEC3F_ZERO;
	}

} // namespace VTX::UI::QT::Controller
