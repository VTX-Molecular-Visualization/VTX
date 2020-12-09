#ifndef __VTX_CONTROLLER_TRACKBALL__
#define __VTX_CONTROLLER_TRACKBALL__

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
		class Trackball : public BaseMouseController, public BaseKeyboardController, public BaseCameraController
		{
		  public:
			// TOFIX: Ugly... set the camera in the BaseCollectionable::init()?
			explicit Trackball() :
				_camera( VTXApp::get().getScene().getCamera() ), _target( VTXApp::get().getScene().getAABB().centroid() ),
				_distanceForced( VTXApp::get().getScene().getAABB().diameter() )
			{
			}

			virtual void setActive( const bool p_active ) override;
			virtual void update( const double & ) override;
			virtual void reset() override;
			virtual void focus( const Math::AABB & ) override;

			inline const Vec3d & getTarget() const { return _target; }

		  protected:
		  private:
			Object3D::Camera & _camera;

			Vec3d  _target		   = VEC3D_ZERO;
			double _distanceForced = 0.0;
			Vec3f  _velocity	   = VEC3F_ZERO;

			bool _needUpdate = true;
		};
	} // namespace Controller
} // namespace VTX
#endif
