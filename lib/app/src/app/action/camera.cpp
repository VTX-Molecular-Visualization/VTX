#include "app/action/camera.hpp"
#include "app/animation/orient.hpp"
#include "app/animation/straight_travel.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include <app/application/scene.hpp>
#include <app/component/controller.hpp>
#include <app/component/render/camera.hpp>
#include <util/collection.hpp>

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
	void Orient::execute()
	{
		Component::Controller & component = ECS_REGISTRY().getComponent<Component::Controller>( SCENE().getCamera() );
		component.launchAnimation( App::Animation::Orient( App::SCENE().getCamera(), _target ) );
	}
	Orient::Orient() : _target( App::SCENE().getAABB() ) {}

	void MoveCamera::execute()
	{
		Component::Controller & component = ECS_REGISTRY().getComponent<Component::Controller>( SCENE().getCamera() );
		component.launchAnimation( Animation::StraightTravel( _position, _rotation, _duration ) );
	}

	void SetCameraPosition::execute() {}

} // namespace VTX::App::Action::Camera
