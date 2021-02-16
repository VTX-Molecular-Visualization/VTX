#include "trackball.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/selection.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "tool/logger.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Controller
	{
		void Trackball::setActive( const bool p_active )
		{
			BaseController::setActive( p_active );
			if ( p_active )
			{
				_target = _camera.getPosition() + _camera.getFront() * _distanceForced;
			}
			else
			{
				_velocity = VEC3F_ZERO;
				// Save distance to force at next setActive(true).
				// If orient is called in Freefly, the distance is overriden.
				_distanceForced = Util::Math::distance( _camera.getPosition(), _target );
			}
		}

		void Trackball::_updateInputs( const float & p_deltaTime )
		{
			// Deselect
			if ( _mouseLeftClick )
			{
				VTX_ACTION(
					new Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
			}

			// Wheel.
			float deltaDistance = 0.f;
			if ( _deltaMouseWheel != 0.f )
			{
				deltaDistance	 = _deltaMouseWheel * 10.0f * p_deltaTime;
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
			if ( _isKeyPressed( Qt::Key_Z ) || _isKeyPressed( Qt::Key_Up ) )
			{
				deltaDistance = 1.5f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_S ) || _isKeyPressed( Qt::Key_Down ) )
			{
				deltaDistance = -1.5f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_D ) || _isKeyPressed( Qt::Key_Right ) )
			{
				deltaVelocity.x = 1e4f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_Q ) || _isKeyPressed( Qt::Key_Left ) )
			{
				deltaVelocity.x = -1e4f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_R ) )
			{
				deltaVelocity.y = 1e4f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_F ) )
			{
				deltaVelocity.y = -1e4f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_E ) )
			{
				deltaVelocity.z = 1e4f * p_deltaTime;
			}
			if ( _isKeyPressed( Qt::Key_A ) )
			{
				deltaVelocity.z = -1e4f * p_deltaTime;
			}

			// Set values from settings.
			if ( deltaDistance != 0.f )
			{
				deltaDistance *= VTX_SETTING().translationSpeed;

				if ( _isKeyPressed( Qt::Key_Shift ) )
				{
					deltaDistance *= VTX_SETTING().translationFactorSpeed;
				}
				if ( _isKeyPressed( Qt::Key_Control ) )
				{
					deltaDistance /= VTX_SETTING().translationFactorSpeed;
				}

				_needUpdate = true;
			}

			if ( deltaVelocity != VEC3F_ZERO )
			{
				_velocity.x += VTX_SETTING().rotationSpeed * deltaVelocity.x;
				_velocity.y
					+= VTX_SETTING().rotationSpeed * deltaVelocity.y * ( VTX_SETTING().yAxisInverted ? -1.f : 1.f );
				_velocity.z += VTX_SETTING().rotationSpeed * deltaVelocity.z;
			}

			_needUpdate |= _velocity != VEC3F_ZERO;

			// Update if needed.
			if ( _needUpdate )
			{
				float distance = 0.f;
				if ( _distanceForced != 0.f )
				{
					distance		= Util::Math::clamp( _distanceForced - deltaDistance, 0.1f, 10000.f );
					_distanceForced = 0.f;
				}
				else
				{
					distance = Util::Math::distance( _camera.getPosition(), _target );
					distance = Util::Math::clamp( distance - deltaDistance, 0.1f, 10000.f );
				}

				Quatf rotation = Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z ) * p_deltaTime );
				_camera.rotateAround( rotation, _target, distance );
				float d = Util::Math::distance( _camera.getPosition(), _target );
				// VTX_LOG_FILE( std::to_string( p_deltaTime ) + " / " + std::to_string( distance ) + " / "
				//			  + std::to_string( d ) );
				_needUpdate = false;
			}

			// Handle elasticity.
			_updateElasticity( p_deltaTime );
		}

		void Trackball::_updateElasticity( const float & p_deltaTime )
		{
			if ( _velocity != VEC3F_ZERO )
			{
				_velocity = Util::Math::linearInterpolation(
					_velocity, VEC3F_ZERO, p_deltaTime * Setting::CONTROLLER_ELASTICITY_FACTOR );

				Vec3f::bool_type res = Util::Math::lessThan( Util::Math::abs( _velocity ),
															 Vec3f( Setting::CONTROLLER_ELASTICITY_THRESHOLD ) );
				if ( !_mouseLeftPressed && res.x && res.y && res.z )
				{
					_velocity = VEC3F_ZERO;
				}
			}
		}

		void Trackball::reset()
		{
			_needUpdate			   = true;
			const Vec3f defaultPos = -CAMERA_FRONT_DEFAULT * VTXApp::get().getScene().getAABB().radius()
									 / ( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			_camera.setPosition( defaultPos );
			_camera.setRotation( Vec3f( 0.f, 0.f, 0.f ) );
			_target	  = VTXApp::get().getScene().getAABB().centroid();
			_velocity = VEC3F_ZERO;
		}

		void Trackball::_computeOrientPositions( const Math::AABB & p_aabb )
		{
			_orientStartingPosition = _target;
			_orientTargetPosition	= p_aabb.centroid();
			_velocity				= VEC3F_ZERO;
			_orientStartingDistance = Util::Math::distance( _camera.getPosition(), _target );
			_orientTargetDistance	= p_aabb.radius() / ( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			_isOrienting = Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD
						   || abs( _orientTargetDistance - _orientStartingDistance ) > ORIENT_THRESHOLD;
		}

		void Trackball::_updateOrient( const float & p_deltaTime )
		{
			_target = Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime );
			float distance
				= Util::Math::easeInOutInterpolation( _orientStartingDistance, _orientTargetDistance, p_deltaTime );
			_camera.rotateAround( QUATF_ID, _target, distance );
		}

	} // namespace Controller
} // namespace VTX
