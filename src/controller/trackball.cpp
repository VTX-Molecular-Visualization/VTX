#include "trackball.hpp"
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
				_needUpdate		= true;
				_target			= VTXApp::get().getScene().getAABB().centroid();
				_distanceForced = VTXApp::get().getScene().getAABB().diameter();
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
				deltaVelocity.x = -(float)_deltaMousePosition.x * 15.f;
				deltaVelocity.y = (float)_deltaMousePosition.y * 15.f;
			}
			// Mouse right.
			else if ( _mouseRightPressed )
			{
				deltaVelocity.z = -(float)_deltaMousePosition.x * 15.f;
			}
			// Pan target with wheel button.
			else if ( _mouseMiddlePressed )
			{
				double deltaX = _deltaMousePosition.x * 0.1;
				double deltaY = _deltaMousePosition.y * 0.1;

				_target		= _target + _camera.getRotation() * ( -VEC3D_X * deltaX + VEC3D_Y * deltaY );
				_needUpdate = true;
			}
			_deltaMousePosition.x = 0;
			_deltaMousePosition.y = 0;

			// Keyboard.
			if ( _isKeyPressed( SDL_SCANCODE_W ) || _isKeyPressed( SDL_SCANCODE_UP ) )
			{
				deltaDistance = 1.5f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_S ) || _isKeyPressed( SDL_SCANCODE_DOWN ) )
			{
				deltaDistance = -1.5f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_A ) || _isKeyPressed( SDL_SCANCODE_LEFT ) )
			{
				deltaVelocity.x = 1e4f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_D ) || _isKeyPressed( SDL_SCANCODE_RIGHT ) )
			{
				deltaVelocity.x = -1e4f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_R ) )
			{
				deltaVelocity.y = -1e4f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_F ) )
			{
				deltaVelocity.y = 1e4f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_Q ) )
			{
				deltaVelocity.z = 1e4f * (float)p_deltaTime;
			}
			if ( _isKeyPressed( SDL_SCANCODE_E ) )
			{
				deltaVelocity.z = -1e4f * (float)p_deltaTime;
			}

			// Set values from settings.
			if ( deltaDistance != 0.f )
			{
				deltaDistance *= VTX_SETTING().translationSpeed;

				if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) )
				{
					deltaDistance *= VTX_SETTING().translationFactorSpeed;
				}
				if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) )
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
				double distance = 0.0;
				if ( _distanceForced != 0.0 )
				{
					distance		= Util::Math::clamp( _distanceForced - deltaDistance, 0.1, 10000.0 );
					_distanceForced = 0.0;
				}
				else
				{
					distance = Util::Math::distance( _camera.getPosition(), _target );
					distance = Util::Math::clamp( distance - deltaDistance, 0.1, 10000.0 );
				}

				Quatd rotation = Quatd( Vec3d( -_velocity.y, _velocity.x, -_velocity.z ) * p_deltaTime );
				_camera.rotateAround( rotation, _target, distance );

				/***********************
				// Orbit backup.
				_rotationYAxis += _velocityX;
				_rotationXAxis -= _velocityY;

				Quatf rotation = Quatf( Vec3f( _rotationXAxis, _rotationYAxis, 0.f ) );
				Vec3f position = rotation * Vec3f( 0.f, 0.f, _distance ) + _target;
				************************/

				_needUpdate = false;
			}

			// Handle elasticity.
			if ( _velocity != VEC3F_ZERO )
			{
				_velocity = Util::Math::linearInterpolation(
					_velocity, VEC3F_ZERO, (float)p_deltaTime * Setting::CONTROLLER_ELASTICITY_FACTOR );

				Vec3f::bool_type res = Util::Math::lessThan( Util::Math::abs( _velocity ),
															 Vec3f( Setting::CONTROLLER_ELASTICITY_THRESHOLD,
																	Setting::CONTROLLER_ELASTICITY_THRESHOLD,
																	Setting::CONTROLLER_ELASTICITY_THRESHOLD ) );
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
