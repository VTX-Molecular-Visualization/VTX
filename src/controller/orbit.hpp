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

			virtual void				update( const double ) override;
			virtual const std::string & getName() const override { return ID::Controller::ORBIT; }
			virtual void				setActive( const bool p_active ) override;

		  protected:
			virtual void					_handleKeyPressedEvent( const SDL_Scancode & ) override;
			virtual std::vector<ID::VTX_ID> _getUIItems() const override
			{
				return std::vector<ID::VTX_ID>( { ID::UI::RENDER } );
			}

		  private:
			Object3D::Camera & _camera;
			Vec3f			   _target = VEC3F_ZERO;

			float _distance		 = 100.f;
			float _velocityX	 = 0.f;
			float _velocityY	 = 0.f;
			float _rotationXAxis = 0.f;
			float _rotationYAxis = 0.f;

			bool _needUpdate = true;
		};
	} // namespace Controller
} // namespace VTX
#endif
