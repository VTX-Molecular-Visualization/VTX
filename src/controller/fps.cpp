#include "fps.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Controller
	{
		void FPS::update( const double p_deltaTime )
		{
			Vec3f translation = VEC3F_ZERO;

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

		void FPS::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE ) { _camera.print(); }
		}

		void FPS::_handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event )
		{
			if ( _mouseLeftPressed )
			{
				_camera.rotate( Vec3d( -Setting::Controller::rotationSpeed * p_event.yrel
										   * ( Setting::Controller::yAxisInverted ? -1.f : 1.f ),
									   -Setting::Controller::rotationSpeed * p_event.xrel,

									   0.0 ) );
			}
			if ( _mouseRightPressed ) { _camera.rotateRoll( Setting::Controller::rotationSpeed * p_event.xrel ); }
		}

	} // namespace Controller
} // namespace VTX
