#ifndef __VTX_CONTROLLER_ORBIT__
#define __VTX_CONTROLLER_ORBIT__

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
		class Orbit : public BaseKeyboardController, public BaseMouseController
		{
		  public:
			// TOFIX: Ugly... set the camera in the BaseCollectionable::init()?
			explicit Orbit() : _camera( VTXApp::get().getScene().getCamera() ) {}

			virtual void receiveEvent( const SDL_Event & p_event ) override final
			{
				BaseKeyboardController::receiveEvent( p_event );
				BaseMouseController::receiveEvent( p_event );
			}

			virtual void		update( const double ) override;
			virtual std::string getName() const override { return ID::Controller::ORBIT; }

		  protected:
			virtual void _handleKeyPressedEvent( const SDL_Scancode & ) override;

		  private:
			Object3D::Camera & _camera;
			Vec3f			   _target = VEC3F_ZERO;

			float _velocityX = 0.0f;
			float _velocityY = 0.0f;
		};
	} // namespace Controller
} // namespace VTX
#endif
