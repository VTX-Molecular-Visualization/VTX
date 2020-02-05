#ifndef __VTX_CONTROLLER_FPS__
#define __VTX_CONTROLLER_FPS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "object3d/camera.hpp"

namespace VTX
{
	namespace Controller
	{
		class ControllerFPS : public BaseKeyboardController, public BaseMouseController
		{
		  public:
			explicit ControllerFPS( Object3D::Camera & p_camera ) : _camera( p_camera ) {}

			virtual void handleEvent( const SDL_Event & p_event, void * const p_arg ) override final
			{
				BaseKeyboardController::handleEvent( p_event, p_arg );
				BaseMouseController::handleEvent( p_event, p_arg );
			}

		  protected:
			virtual void _handleKeyDownEvent( const SDL_Scancode & ) override;
			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & ) override;

		  private:
			Object3D::Camera & _camera;
			bool			   _mouseLeftPressed = false;
		};
	} // namespace Controller
} // namespace VTX
#endif
