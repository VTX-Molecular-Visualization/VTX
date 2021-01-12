#ifndef __VTX_CONTROLLER_TRACKBALL__
#define __VTX_CONTROLLER_TRACKBALL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_camera_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class Trackball : public BaseCameraController
		{
		  public:
			explicit Trackball( Object3D::Camera & p_camera, const Vec3f p_target, const float p_distance ) :
				BaseCameraController( p_camera ), _target( p_target ), _distance( p_distance )
			{
			}

			void setActive( const bool p_active ) override;
			void reset() override;

			inline const Vec3f & getTarget() const { return _target; }

		  protected:
			void _updateInputs( const float & ) override;
			void _updateOrient( const float & ) override;

		  private:
			Vec3f _target	= VEC3F_ZERO;
			float _distance = 0.f;
			Vec3f _velocity = VEC3F_ZERO;

			bool _needUpdate = true;
		};
	} // namespace Controller
} // namespace VTX
#endif
