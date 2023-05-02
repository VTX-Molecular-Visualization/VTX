#ifndef __VTX_CONTROLLER_FREEFLY__
#define __VTX_CONTROLLER_FREEFLY__

#include "base_camera_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class Freefly : public BaseCameraController
		{
		  public:
			explicit Freefly( App::Internal::Scene::CameraManager & p_cameraManager ) :
				BaseCameraController( p_cameraManager )
			{
			}

			const ID::VTX_ID getID() const override { return ID::Controller::FREEFLY; }

		  protected:
			void _updateInputs( const float & ) override;
			void _updateOrient( const float & ) override;
			void _computeOrientPositions( const App::Component::Object3D::Helper::AABB & ) override;
			void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) override;
		};
	} // namespace Controller
} // namespace VTX
#endif
