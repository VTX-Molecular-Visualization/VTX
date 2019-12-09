#ifndef __VTX_CONTROLLER_FREE_FLY__
#define __VTX_CONTROLLER_FREE_FLY__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/camera.hpp"
#include "base_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class ControllerFreeFly : public BaseController
		{
		  public:
			ControllerFreeFly( Object3D::Camera & p_camera ) : _camera( p_camera ) {}

		  protected:
			virtual void _handleKeyDownEvent( const SDL_Scancode & ) override;
			virtual void _handleKeyUpEvent( const SDL_Scancode & ) override;
			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent & ) override;
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & ) override;
			virtual void _handleMouseWheelEvent( const SDL_MouseWheelEvent & ) override;

		  private:
			Object3D::Camera & _camera;
		};
	} // namespace Controller
} // namespace VTX
#endif
