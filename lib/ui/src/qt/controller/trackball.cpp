#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/style.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <app/old_app/style.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Controller
{
	void Trackball::setActive( const bool p_active )
	{
		BaseController::setActive( p_active );
		if ( p_active )
		{
			_target = targetSimulationFromCamera( _camera );
		}
		else
		{
			_velocity = VEC3F_ZERO;
			// Save distance to force at next setActive(true).
			// If orient is called in Freefly, the distance is overriden.
			_distanceForced = VTX::Util::Math::distance( _camera.getPosition(), _target );
		}
	}

	Vec3f Trackball::targetSimulationFromCamera( const Object3D::Camera & p_camera ) const
	{
		return p_camera.getPosition() + p_camera.getFront() * _distanceForced;
	}

	void Trackball::_updateInputs( const float & p_deltaTime )
	{
		// Wheel.
		float deltaDistance = 0.f;
		if ( _deltaMouseWheel != 0.f )
		{
			deltaDistance	 = _deltaMouseWheel * 0.00001 * VTX::Util::Math::distance( _camera.getPosition(), _target );
			_deltaMouseWheel = 0;
		}

		// Mouse left.
		Vec3f deltaVelocity = VEC3F_ZERO;
		if ( _mouseLeftPressed )
		{
			deltaVelocity.x = -_deltaMousePosition.x * 15.f;
			deltaVelocity.y = -_deltaMousePosition.y * 15.f;
		}
		// Mouse right.
		else if ( _mouseRightPressed )
		{
			deltaVelocity.z = _deltaMousePosition.x * 15.f;
		}
		// Pan target with wheel button.
		else if ( _mouseMiddlePressed )
		{
			float deltaX = -_deltaMousePosition.x * 0.1;
			float deltaY = _deltaMousePosition.y * 0.1;

			_target		= _target + _camera.getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
			_needUpdate = true;
		}
		_deltaMousePosition.x = 0;
		_deltaMousePosition.y = 0;

		// Keyboard.
		if ( _isKeyPressed( getKeyboardLayout() == KeyboardLayout::QWERTY ? Qt::Key::Key_W : Qt::Key::Key_Z )
			 || _isKeyPressed( Qt::Key::Key_Up ) )
		{
			deltaDistance = 1.5f * p_deltaTime;
		}
		if ( _isKeyPressed( Qt::Key::Key_S ) || _isKeyPressed( Qt::Key::Key_Down ) )
		{
			deltaDistance = -1.5f * p_deltaTime;
		}
		if ( _isKeyPressed( Qt::Key::Key_D ) || _isKeyPressed( Qt::Key::Key_Right ) )
		{
			deltaVelocity.x = 1e4f * p_deltaTime;
		}
		if ( _isKeyPressed( getKeyboardLayout() == KeyboardLayout::QWERTY ? Qt::Key::Key_A : Qt::Key::Key_Q )
			 || _isKeyPressed( Qt::Key::Key_Left ) )
		{
			deltaVelocity.x = -1e4f * p_deltaTime;
		}
		if ( _isKeyPressed( Qt::Key::Key_R ) )
		{
			deltaVelocity.y = 1e4f * p_deltaTime;
		}
		if ( _isKeyPressed( Qt::Key::Key_F ) )
		{
			deltaVelocity.y = -1e4f * p_deltaTime;
		}
		if ( _isKeyPressed( Qt::Key::Key_E ) )
		{
			deltaVelocity.z = 1e4f * p_deltaTime;
		}
		if ( _isKeyPressed( getKeyboardLayout() == KeyboardLayout::QWERTY ? Qt::Key::Key_Q : Qt::Key::Key_A ) )
		{
			deltaVelocity.z = -1e4f * p_deltaTime;
		}

		// Set values from settings.
		if ( deltaDistance != 0.f )
		{
			deltaDistance *= VTX_SETTING().getTranslationSpeed();

			if ( _isModifierExclusive( ModifierFlag::Shift ) )
			{
				deltaDistance *= VTX_SETTING().getAccelerationSpeedFactor();
			}
			if ( _isModifierExclusive( ModifierFlag::Alt ) )
			{
				deltaDistance /= VTX_SETTING().getDecelerationSpeedFactor();
			}

			_needUpdate = true;
		}

		if ( deltaVelocity != VEC3F_ZERO )
		{
			if ( _isModifierExclusive( ModifierFlag::Shift ) )
			{
				deltaVelocity *= VTX_SETTING().getAccelerationSpeedFactor();
			}
			if ( _isModifierExclusive( ModifierFlag::Alt ) )
			{
				deltaVelocity /= VTX_SETTING().getDecelerationSpeedFactor();
			}

			_velocity.x += VTX_SETTING().getRotationSpeed() * deltaVelocity.x;
			_velocity.y += VTX_SETTING().getRotationSpeed() * deltaVelocity.y
						   * ( VTX_SETTING().getYAxisInverted() ? -1.f : 1.f );
			_velocity.z += VTX_SETTING().getRotationSpeed() * deltaVelocity.z;
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
				distance = VTX::Util::Math::distance( _camera.getPosition(), _target );
				distance = VTX::Util::Math::clamp( distance - deltaDistance, 0.1f, 10000.f );
			}

			const Quatf rotation = Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z )
										  * ( VTX_SETTING().getControllerElasticityActive() ? p_deltaTime : 0.2f ) );
			_camera.rotateAround( rotation, _target, distance );
			// float d = Util::distance( _camera.getPosition(), _target );
			// VTX_LOG_FILE( std::to_string( p_deltaTime ) + " / " + std::to_string( distance ) + " / "
			//			  + std::to_string( d ) );
			_needUpdate = false;
		}

		// Handle elasticity.
		if ( VTX_SETTING().getControllerElasticityActive() )
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
			_velocity = VTX::Util::Math::linearInterpolation(
				_velocity, VEC3F_ZERO, p_deltaTime * VTX_SETTING().getControllerElasticityFactor() );

			Vec3f::bool_type res = VTX::Util::Math::lessThan( VTX::Util::Math::abs( _velocity ),
															  Vec3f( Setting::CONTROLLER_ELASTICITY_THRESHOLD ) );
			if ( !_mouseLeftPressed && res.x && res.y && res.z )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}

	void Trackball::reset()
	{
		BaseCameraController::reset();

		_needUpdate = true;
		_target		= VTXApp::get().getScene().getAABB().centroid();
		_velocity	= VEC3F_ZERO;
	}

	void Trackball::_computeOrientPositions( const Object3D::Helper::AABB & p_aabb )
	{
		_orientStartingPosition = _target;
		_orientTargetPosition	= p_aabb.centroid();
		_velocity				= VEC3F_ZERO;

		_orientStartingRotation = _camera.getRotation();
		_orientTargetRotation	= _camera.getRotation();

		_orientStartingDistance = VTX::Util::Math::distance( _camera.getPosition(), _target );
		_orientTargetDistance
			= p_aabb.radius()
			  / (float)( tan( VTX::Util::Math::radians( _camera.getFov() ) * Style::ORIENT_ZOOM_FACTOR ) );
		_isOrienting = VTX::Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD
					   || abs( _orientTargetDistance - _orientStartingDistance ) > ORIENT_THRESHOLD;
	}

	void Trackball::_computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation )
	{
		_orientStartingDistance = VTX::Util::Math::distance( _camera.getPosition(), _target );
		_orientTargetDistance	= VTX::Util::Math::distance( p_position, _target );

		_orientStartingPosition = _camera.getPosition() + _camera.getFront() * _orientStartingDistance;
		_orientTargetPosition	= _target;

		_orientStartingRotation = _camera.getRotation();
		_orientTargetRotation	= p_orientation;

		_velocity	 = VEC3F_ZERO;
		_isOrienting = VTX::Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD
					   || abs( _orientTargetDistance - _orientStartingDistance ) > ORIENT_THRESHOLD;
	}

	void Trackball::_updateOrient( const float & p_deltaTime )
	{
		_target
			= VTX::Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime );
		float distance
			= VTX::Util::Math::easeInOutInterpolation( _orientStartingDistance, _orientTargetDistance, p_deltaTime );
		_camera.rotateAround( QUATF_ID, _target, distance );

		_camera.setRotation(
			VTX::Util::Math::easeInOutInterpolation( _orientStartingRotation, _orientTargetRotation, p_deltaTime ) );
	}

} // namespace VTX::UI::QT::Controller
