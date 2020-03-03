#include "orbit.hpp"
#include "setting.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Controller
	{
		void Orbit::update( const double p_deltaTime )
		{
			

			if ( _deltaMouseWheel != 0.f )
			{
				float distance = Setting::Controller::translationSpeed * _deltaMouseWheel * 0.01f;

				if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) ) { distance *= Setting::Controller::translationFactorSpeed; }
				if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) ) { distance /= Setting::Controller::translationFactorSpeed; }
				_camera.moveFront( distance );
				_deltaMouseWheel = 0;
			}

			if ( _velocityX != 0.f ) { _camera.rotateAround( _target, VEC3F_Y, _velocityX ); }

			_velocityX = Util::Math::lerp( _velocityX, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY );
			_velocityY = Util::Math::lerp( _velocityY, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY );

			if ( _mouseLeftPressed )
			{
				_velocityX += Setting::Controller::rotationSpeed * (float)_deltaMousePosition.y
							  * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );
				_velocityY += -Setting::Controller::rotationSpeed * (float)_deltaMousePosition.x;
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
		}

		void Orbit::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE ) { _camera.print(); }
		}
	} // namespace Controller
} // namespace VTX
