#ifndef __VTX_CONTROLLER_FREEFLY__
#define __VTX_CONTROLLER_FREEFLY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_camera_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "id.hpp"
#include "object3d/camera.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		class Freefly : public BaseKeyboardController, public BaseMouseController, public BaseCameraController
		{
		  public:
			// TOFIX: Ugly... set the camera in the BaseCollectionable::init()?
			explicit Freefly() : _camera( VTXApp::get().getScene().getCamera() ) {}

			/*
			virtual void receiveEvent( const SDL_Event & p_event ) override final
			{
				BaseKeyboardController::receiveEvent( p_event );
				BaseMouseController::receiveEvent( p_event );
			}
			*/

			virtual void update( const double & ) override;

			virtual void reset() override;

		  protected:
			// virtual void					_handleKeyPressedEvent( const SDL_Scancode & ) override;

		  private:
			Object3D::Camera & _camera;
		};
	} // namespace Controller
} // namespace VTX
#endif
