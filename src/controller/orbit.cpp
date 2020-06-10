#include "orbit.hpp"
#include "setting.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Controller
	{
		void Orbit::setActive( const bool p_active )
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
				_velocityX = 0.f;
				_velocityY = 0.f;
			}
		}

		void Orbit::update( const double p_deltaTime )
		{
			// Wheel.
			float deltaDistance = 0.f;
			if ( _deltaMouseWheel != 0.f )
			{
				deltaDistance	 = Setting::Controller::translationSpeed * _deltaMouseWheel * 0.01f;
				_deltaMouseWheel = 0;
			}

			// Mouse left.
			float deltaVelocityX = 0.f;
			float deltaVelocityY = 0.f;
			if ( _mouseLeftPressed )
			{
				deltaVelocityX = -Setting::Controller::rotationSpeed * (float)_deltaMousePosition.x;
				deltaVelocityY = Setting::Controller::rotationSpeed * (float)_deltaMousePosition.y
								 * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );

				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}

			// Keyboard.
			if ( _isKeyPressed( SDL_SCANCODE_W ) || _isKeyPressed( SDL_SCANCODE_UP ) )
			{
				deltaDistance++;
			}
			if ( _isKeyPressed( SDL_SCANCODE_S ) || _isKeyPressed( SDL_SCANCODE_DOWN ) )
			{
				deltaDistance--;
			}
			if ( _isKeyPressed( SDL_SCANCODE_A ) || _isKeyPressed( SDL_SCANCODE_LEFT ) )
			{
				deltaVelocityX = -Setting::Controller::rotationSpeed * 10.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_D ) || _isKeyPressed( SDL_SCANCODE_RIGHT ) )
			{
				deltaVelocityX = Setting::Controller::rotationSpeed * 10.f;
			}
			if ( _isKeyPressed( SDL_SCANCODE_R ) )
			{
				deltaVelocityY
					= Setting::Controller::rotationSpeed * 10.f * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );
			}
			if ( _isKeyPressed( SDL_SCANCODE_F ) )
			{
				deltaVelocityY
					= -Setting::Controller::rotationSpeed * 10.f * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );
			}

			// Pan target with wheel button.

			// Update.
			if ( deltaDistance != 0.f )
			{
				if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) )
				{
					deltaDistance *= Setting::Controller::translationFactorSpeed;
				}
				if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) )
				{
					deltaDistance /= Setting::Controller::translationFactorSpeed;
				}

				_distance	= Util::Math::clamp( _distance - deltaDistance, 10.f, 1000.f );
				_needUpdate = true;
			}
			if ( deltaVelocityX != 0.f )
			{
				_velocityX += Setting::Controller::rotationSpeed * deltaVelocityX * 5.f;
			}
			if ( deltaVelocityY != 0.f )
			{
				_velocityY += Setting::Controller::rotationSpeed * deltaVelocityY * 5.f;
			}

			_needUpdate |= _velocityX != 0.f;
			_needUpdate |= _velocityY != 0.f;

			if ( _needUpdate )
			{
				_rotationYAxis += _velocityX;
				_rotationXAxis -= _velocityY;

				Quatf rotation = Quatf( Vec3f( _rotationXAxis, _rotationYAxis, 0.f ) );
				Vec3f position = rotation * Vec3f( 0.f, 0.f, _distance ) + _target;

				_camera.set( position, rotation );
				_needUpdate = false;
			}

			// Handle elasticity.
			if ( _velocityX != 0.f )
			{
				_velocityX = Util::Math::linearInterpolation(
					_velocityX, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY_FACTOR );

				if ( !_mouseLeftPressed && Util::Math::abs( _velocityX ) < CONTROLLER_ORBIT_ELASTICITY_THRESHOLD )
				{
					_velocityX = 0.f;
				}
			}
			if ( _velocityY != 0.f )
			{
				_velocityY = Util::Math::linearInterpolation(
					_velocityY, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY_FACTOR );

				if ( !_mouseLeftPressed && Util::Math::abs( _velocityY ) < CONTROLLER_ORBIT_ELASTICITY_THRESHOLD )
				{
					_velocityY = 0.f;
				}
			}
		}

		void Orbit::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE )
			{
				_camera.print();
			}
		}
	} // namespace Controller
} // namespace VTX
