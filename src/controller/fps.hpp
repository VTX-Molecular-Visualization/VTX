#ifndef __VTX_CONTROLLER_FPS__
#define __VTX_CONTROLLER_FPS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "id.hpp"
#include "object3d/camera.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		class FPS : public BaseKeyboardController, public BaseMouseController
		{
		  public:
			// TOFIX: Ugly... set the camera in the BaseCollectionable::init()?
			explicit FPS() : _camera( VTXApp::get().getScene().getCamera() ) {}

			virtual void handleEvent( const SDL_Event & p_event, void * const p_arg ) override final
			{
				BaseKeyboardController::handleEvent( p_event, p_arg );
				BaseMouseController::handleEvent( p_event, p_arg );
			}

			virtual void		update( const double ) override;
			virtual std::string getName() const override { return ID::Controller::FPS; }

		  protected:
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & ) override;
			virtual void _handleKeyPressedEvent( const SDL_Scancode & ) override;

		  private:
			Object3D::Camera & _camera;
		};
	} // namespace Controller
} // namespace VTX
#endif
