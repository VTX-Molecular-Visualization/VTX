#include "app/action/camera.hpp"
#include "app/animation/orient.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"

namespace VTX::App::Action::Camera
{

	void SetCameraProjectionOrthographic::execute()
	{
		auto & camera = SCENE().getCamera();
		camera.setCameraProjection( Component::Render::Camera::PROJECTION::ORTHOGRAPHIC );
	}

	void SetCameraProjectionPerspective::execute()
	{
		auto & camera = SCENE().getCamera();
		camera.setCameraProjection( Component::Render::Camera::PROJECTION::PERSPECTIVE );
	}

	void ToggleCameraProjection::execute()
	{
		using namespace Component::Render;
		auto & camera = SCENE().getCamera();
		if ( camera.getProjection() == Component::Render::Camera::PROJECTION::ORTHOGRAPHIC )
		{
			camera.setCameraProjection( Component::Render::Camera::PROJECTION::PERSPECTIVE );
		}
		else
		{
			camera.setCameraProjection( Component::Render::Camera::PROJECTION::ORTHOGRAPHIC );
		}
	}

	void Reset::execute()
	{
		auto & camera = SCENE().getCamera();

		Vec3f position = Animation::Orient::computeCameraOrientPosition(
			camera.getTransform().getFront(),
			camera.getFov(),
			App::SCENE().getAABB(),
			Animation::Orient::ORIENT_ZOOM_FACTOR
		);

		camera.getTransform().setPosition( position );
		camera.getTransform().setRotation( QUATF_ID );
		camera.setTargetWorld( App::SCENE().getAABB().centroid() );
	}

} // namespace VTX::App::Action::Camera
