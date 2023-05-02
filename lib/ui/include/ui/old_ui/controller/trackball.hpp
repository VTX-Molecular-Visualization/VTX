#ifndef __VTX_CONTROLLER_TRACKBALL__
#define __VTX_CONTROLLER_TRACKBALL__

#include "base_camera_controller.hpp"
#include <app/component/render/_fwd.hpp>

namespace VTX::Controller
{
	class Trackball : public BaseCameraController
	{
	  public:
		explicit Trackball( App::Internal::Scene::CameraManager & p_cameraManager,
							const Vec3f							  p_target,
							const float							  p_distance ) :
			BaseCameraController( p_cameraManager ),
			_target( p_target ), _distanceForced( p_distance )
		{
			_camera().setTarget( p_target );
		}

		const ID::VTX_ID getID() const override { return ID::Controller::TRACKBALL; }
		void			 setActive( const bool p_active ) override;
		void			 reset() override;

		inline const Vec3f & getTarget() const { return _target; }
		void				 setTarget( const Vec3f & p_target );
		inline void			 setDistanceForced( const float & p_distance ) { _distanceForced = p_distance; }

		Vec3f targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const;

	  protected:
		void _updateInputs( const float & ) override;
		void _updateOrient( const float & ) override;
		void _computeOrientPositions( const App::Component::Object3D::Helper::AABB & ) override;
		void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) override;

	  private:
		Vec3f _target		  = VEC3F_ZERO;
		float _distanceForced = 0.f;
		Vec3f _velocity		  = VEC3F_ZERO;

		float _orientStartingDistance = 0.f;
		float _orientTargetDistance	  = 0.f;

		bool _needUpdate = true;

		void _updateElasticity( const float & );
	};
} // namespace VTX::Controller
#endif
