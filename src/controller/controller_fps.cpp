#include "controller_fps.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerFPS::_handleKeyDownEvent( const SDL_Scancode & p_code )
		{
			switch ( p_code )
			{
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP: _camera.moveFront( Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_LEFT: _camera.moveLeft( Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_DOWN: _camera.moveFront( -Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT: _camera.moveLeft( -Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_R: _camera.moveUp( Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_F: _camera.moveUp( -Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_Q: _camera.rotateLeft( Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_E: _camera.rotateLeft( Setting::Controller::translationSpeed ); break;
			case SDL_SCANCODE_SPACE: _camera.print(); break;
			}
		}

		void ControllerFPS::_handleMouseButtonDownEvent( const SDL_MouseButtonEvent & p_event )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = true;
			default: break;
			}
		}

		void ControllerFPS::_handleMouseButtonUpEvent( const SDL_MouseButtonEvent & p_event )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = false;
			default: break;
			}
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
