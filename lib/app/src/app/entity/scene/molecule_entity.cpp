#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/component/behaviour/molecule_behaviour.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include "app/component/render/proxy_molecule.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/entity/scene/scene_item_entity.hpp"
#include <renderer/facade.hpp>
#include <renderer/proxy/molecule.hpp>

namespace VTX::App::Entity::Scene
{
	void MoleculeEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::addComponent( p_entity, p_extraData );

		MAIN_REGISTRY().addComponent<Component::Chemistry::Molecule>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::AABB>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Render::ProxyMolecule>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::UIDComponent>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Selectable>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Pickable>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Behaviour::Molecule>( p_entity );
	}
	void MoleculeEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::setup( p_entity, p_extraData );

		Component::Behaviour::Molecule & moleculeBehaviour
			= MAIN_REGISTRY().getComponent<Component::Behaviour::Molecule>( p_entity );

		moleculeBehaviour.init( p_extraData );
	}
	void MoleculeEntityBuilder::postSetup(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::postSetup( p_entity, p_extraData );
	}
} // namespace VTX::App::Entity::Scene
