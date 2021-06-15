#ifndef __VTX_CONTROLLER_TRACKBALL__
#define __VTX_CONTROLLER_TRACKBALL__

#include "base_camera_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class Trackball : public BaseCameraController
		{
		  public:
			explicit Trackball( Object3D::Camera & p_camera, const Vec3f p_target, const float p_distance ) :
				BaseCameraController( p_camera ), _target( p_target ), _distanceForced( p_distance )
			{
			}

			void setActive( const bool p_active ) override;
			void reset() override;

			inline const Vec3f & getTarget() const { return _target; }
			inline void			 setTarget( const Vec3f & p_target ) { _target = p_target; }
			inline void			 setDistanceForced( const float & p_distance ) { _distanceForced = p_distance; }

		  protected:
			void _updateInputs( const float & ) override;
			void _updateOrient( const float & ) override;
			void _computeOrientPositions( const Math::AABB & ) override;

		  private:
			Vec3f _target		  = VEC3F_ZERO;
			float _distanceForced = 0.f;
			Vec3f _velocity		  = VEC3F_ZERO;

			float _orientStartingDistance = 0.f;
			float _orientTargetDistance	  = 0.f;

			bool _needUpdate = true;

			void _updateElasticity( const float & );
		};
	} // namespace Controller
} // namespace VTX
#endif
