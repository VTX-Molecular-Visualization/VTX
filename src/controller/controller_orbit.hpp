#ifndef __VTX_CONTROLLER_ORBIT__
#define __VTX_CONTROLLER_ORBIT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/camera.hpp"
#include "base_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class ControllerOrbit : public BaseController
		{
		  public:
			ControllerOrbit( Object3D::Camera & p_camera ) : _camera( p_camera ) {}

		  protected:
			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & ) override;
			virtual void _handleMouseWheelEvent( const SDL_MouseWheelEvent & ) override;

		  private:
			Object3D::Camera & _camera;
			bool			   _mouseLeftPressed  = false;
			bool			   _mouseRightPressed = false;
		};
	} // namespace Controller
} // namespace VTX
#endif
