#include "app/action/camera.hpp"
#include <util/math/transform.hpp>

namespace VTX::App::Action::Camera
{

	void SetPosition::execute( const Vec3f & p_position )
	{
		auto [ _, cam, transform ] = ECS::getFirstEntityWithComponents<App::Scene::Camera, Util::Math::Transform>();
		transform.setPosition( p_position );
	}

	void SetRotation::execute( const Vec3f & p_eulerAngles )
	{
		auto [ _, cam, transform ] = ECS::getFirstEntityWithComponents<App::Scene::Camera, Util::Math::Transform>();
		transform.setRotation( p_eulerAngles );
	}

	void SetScale::execute( const float p_scale )
	{
		auto [ _, cam, transform ] = ECS::getFirstEntityWithComponents<App::Scene::Camera, Util::Math::Transform>();
		transform.setScale( p_scale );
	}
} // namespace VTX::App::Action::Camera
