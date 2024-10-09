#include "app/entity/scene/scene_item_entity.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Scene
{
	void SceneItemEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		MAIN_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( p_entity );
	}

	void SceneItemEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData )
	{
	}
	void SceneItemEntityBuilder::postSetup(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		Application::Scene * const			   scene = p_extraData.at( "scene" ).getPtr<Application::Scene>();
		Component::Scene::SceneItemComponent & sceneItemComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( p_entity );

		scene->referenceItem( sceneItemComponent );
	}
} // namespace VTX::App::Entity::Scene
