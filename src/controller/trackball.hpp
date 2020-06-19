#ifndef __VTX_CONTROLLER_TRACKBALL__
#define __VTX_CONTROLLER_TRACKBALL__

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
		class Trackball : public BaseKeyboardController, public BaseMouseController
		{
		  public:
			// TOFIX: Ugly... set the camera in the BaseCollectionable::init()?
			explicit Trackball() :
				_camera( VTXApp::get().getScene().getCamera() ),
				_target( VTXApp::get().getScene().getAABB().centroid() ),
				_distanceForced( VTXApp::get().getScene().getAABB().diameter() )
			{
			}

			virtual void receiveEvent( const SDL_Event & p_event ) override final
			{
				BaseKeyboardController::receiveEvent( p_event );
				BaseMouseController::receiveEvent( p_event );
			}

			virtual void				update( const double ) override;
			virtual const std::string & getName() const override { return ID::Controller::TRACKBALL; }
			virtual void				setActive( const bool p_active ) override;

		  protected:
			virtual void					_handleKeyPressedEvent( const SDL_Scancode & ) override;
			virtual std::vector<ID::VTX_ID> _getUIItems() const override
			{
				return std::vector<ID::VTX_ID>( { ID::UI::RENDER } );
			}

		  private:
			Object3D::Camera & _camera;

			Vec3f _target		  = VEC3F_ZERO;
			float _distanceForced = 0.f;
			Vec3f _velocity		  = VEC3F_ZERO;

			bool _needUpdate = true;
		};
	} // namespace Controller
} // namespace VTX
#endif
