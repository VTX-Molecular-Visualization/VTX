#ifndef __VTX_CONTROLLER_FREEFLY__
#define __VTX_CONTROLLER_FREEFLY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_keyboard_mouse_controller.hpp"
#include "object3d/camera.hpp"

namespace VTX
{
	namespace Controller
	{
		class ControllerFreefly : public BaseKeyboardMouseController
		{
		  public:
			explicit ControllerFreefly( Object3D::Camera & p_camera ) : _camera( p_camera ) {}

		  protected:
			virtual void _handleKeyDownEvent( const SDL_Scancode &, const double ) override;
			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent &, const double ) override;
			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent &, const double ) override;
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent &, const double ) override;

		  private:
			Object3D::Camera & _camera;
			bool			   _mouseLeftPressed = false;
		};
	} // namespace Controller
} // namespace VTX
#endif
