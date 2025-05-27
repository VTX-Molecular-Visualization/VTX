#include "app/action/scene.hpp"
#include "app/action/camera.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/system.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/render/viewpoint.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/core/network/network_system.hpp"
#include "app/entity/system.hpp"
#include "app/entity/viewpoint.hpp"
#include "app/filesystem.hpp"
#include <util/filesystem.hpp>

namespace VTX::App::Action::Scene
{

	void LoadSystem::execute()
	{
		const auto entity = ECS_REGISTRY().createEntity<Entity::System>( _path.string(), _buffer );
		ACTION_SYSTEM().execute<App::Action::Camera::Orient>( App::SCENE().getAABB() );
	}

	CreateViewpoint::CreateViewpoint() : CreateViewpoint( SCENE().getCamera() ) {}
	CreateViewpoint::CreateViewpoint( const Component::Render::Camera & p_camera ) :
		CreateViewpoint( p_camera.getTransform().getPosition(), p_camera.getTransform().getRotation() )
	{
	}
	void CreateViewpoint::execute()
	{
		const auto entity = ECS_REGISTRY().createEntity<Entity::Viewpoint>();

		auto & viewpoint = ECS_REGISTRY().getComponent<Component::Render::Viewpoint>( entity );
		viewpoint.setPosition( _position );
		viewpoint.setRotation( _rotation );
	}

	void ClearScene::execute() { SCENE().reset(); }

} // namespace VTX::App::Action::Scene
