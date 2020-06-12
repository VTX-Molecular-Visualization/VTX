#include "trackball.hpp"
#include "setting.hpp"
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
				_needUpdate = true;
				_target		= VTXApp::get().getScene().getAABB().centroid();
				_distance	= VTXApp::get().getScene().getAABB().diameter();
			}
			else
			{
				_velocity = VEC3F_ZERO;
			}
		}

		void Trackball::update( const double p_deltaTime )
		{
			// Wheel.
			float deltaDistance = 0.f;
			if ( _deltaMouseWheel != 0.f )
			{
				deltaDistance	 = _deltaMouseWheel * 0.01f;
				_deltaMouseWheel = 0;
			}

			// Mouse left.
			Vec3f deltaVelocity = VEC3F_ZERO;
			if ( _mouseLeftPressed )
			{
				deltaVelocity.x		  = -(float)_deltaMousePosition.x * 0.1f;
				deltaVelocity.y		  = (float)_deltaMousePosition.y * 0.1f;
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
			// Mouse right.
			else if ( _mouseRightPressed )
			{
				deltaVelocity.z		  = -(float)_deltaMousePosition.x * 0.1f;
				_deltaMousePosition.x = 0;
			}
			// Pan target with wheel button.
			else if ( _mouseMiddlePressed )
			{
				float deltaX		  = (float)_deltaMousePosition.x * 0.1f;
				float deltaY		  = (float)_deltaMousePosition.y * 0.1f;
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;

				_target		= _target + _rotation * ( -VEC3F_X * deltaX + VEC3F_Y * deltaY );
				_needUpdate = true;
			}

			// Keyboard.
			if ( _isKeyPressed( SDL_SCANCODE_W ) || _isKeyPressed( SDL_SCANCODE_UP ) )
			{
				deltaDistance = 0.01f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_S ) || _isKeyPressed( SDL_SCANCODE_DOWN ) )
			{
				deltaDistance = -0.01f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_A ) || _isKeyPressed( SDL_SCANCODE_LEFT ) )
			{
				deltaVelocity.x = 0.5f * (float)p_deltaTime * 144.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_D ) || _isKeyPressed( SDL_SCANCODE_RIGHT ) )
			{
				deltaVelocity.x = -0.5f * (float)p_deltaTime * 144.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_R ) )
			{
				deltaVelocity.y = -0.5f * (float)p_deltaTime * 144.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_F ) )
			{
				deltaVelocity.y = 0.5f * (float)p_deltaTime * 144.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_Q ) )
			{
				deltaVelocity.z = 0.5f * (float)p_deltaTime * 144.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_E ) )
			{
				deltaVelocity.z = -0.5f * (float)p_deltaTime * 144.f;
			}

			// Set values from settings.
			if ( deltaDistance != 0.f )
			{
				deltaDistance *= Setting::Controller::translationSpeed;
				deltaDistance *= (float)p_deltaTime * 144.f;

				if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) )
				{
					deltaDistance *= Setting::Controller::translationFactorSpeed;
				}
				if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) )
				{
					deltaDistance /= Setting::Controller::translationFactorSpeed;
				}

				_needUpdate = true;
			}

			if ( deltaVelocity != VEC3F_ZERO )
			{
				_velocity.x += Setting::Controller::rotationSpeed * deltaVelocity.x;
				_velocity.y += Setting::Controller::rotationSpeed * deltaVelocity.y
							   * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );
				_velocity.z += Setting::Controller::rotationSpeed * deltaVelocity.z;
			}

			_needUpdate |= _velocity != VEC3F_ZERO;

			// Update if needed.
			if ( _needUpdate )
			{
				_distance = Util::Math::clamp( _distance - deltaDistance, 0.1f, 10000.f );

				Quatf rotation = Quatf( Vec3f( -_velocity.y, _velocity.x, -_velocity.z ) * (float)p_deltaTime * 144.f );
				_rotation	   = _rotation * rotation;
				Vec3f position = _rotation * Vec3f( 0.f, 0.f, _distance ) + _target;

				_camera.set( position, _rotation );
				_needUpdate = false;
			}

			// Handle elasticity.
			if ( _velocity != VEC3F_ZERO )
			{
				_velocity = Util::Math::linearInterpolation(
					_velocity, VEC3F_ZERO, (float)p_deltaTime * CONTROLLER_ELASTICITY_FACTOR );

				Vec3f::bool_type res = Util::Math::lessThan( Util::Math::abs( _velocity ),
															 Vec3f( CONTROLLER_ELASTICITY_THRESHOLD,
																	CONTROLLER_ELASTICITY_THRESHOLD,
																	CONTROLLER_ELASTICITY_THRESHOLD ) );
				if ( !_mouseLeftPressed && res.x && res.y && res.z )
				{
					_velocity = VEC3F_ZERO;
				}
			}
		}

		void Trackball::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE )
			{
				_camera.print();
			}
		}
	} // namespace Controller
} // namespace VTX
