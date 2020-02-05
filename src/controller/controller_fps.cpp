#include "controller_fps.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerFPS::update( const double p_deltaTime )
		{
			Vec3f translation = Vec3f( 0.f, 0.f, 0.f );

			if ( _isKeyPressed( SDL_SCANCODE_W ) || _isKeyPressed( SDL_SCANCODE_UP ) ) { translation.z++; }
			if ( _isKeyPressed( SDL_SCANCODE_S ) || _isKeyPressed( SDL_SCANCODE_DOWN ) ) { translation.z--; }
			if ( _isKeyPressed( SDL_SCANCODE_A ) || _isKeyPressed( SDL_SCANCODE_LEFT ) ) { translation.x++; }
			if ( _isKeyPressed( SDL_SCANCODE_D ) || _isKeyPressed( SDL_SCANCODE_RIGHT ) ) { translation.x--; }
			if ( _isKeyPressed( SDL_SCANCODE_R ) ) { translation.y++; }
			if ( _isKeyPressed( SDL_SCANCODE_F ) ) { translation.y--; }

			if ( translation == VEC3F_ZERO ) { return; }

			translation *= Setting::Controller::translationSpeed;
			translation *= p_deltaTime;

			if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) ) { translation *= Setting::Controller::translationFactorSpeed; }
			if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) ) { translation /= Setting::Controller::translationFactorSpeed; }

			_camera.move( translation );
		}

		void ControllerFPS::_handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event )
		{
			if ( _mouseLeftPressed )
			{
				_camera.rotateLeft( Setting::Controller::rotationSpeed * p_event.xrel );
				_camera.rotateUp( Setting::Controller::rotationSpeed * p_event.yrel
								  * ( Setting::Controller::yAxisInverted ? -1.f : 1.f ) );
			}
		}

	} // namespace Controller
} // namespace VTX
