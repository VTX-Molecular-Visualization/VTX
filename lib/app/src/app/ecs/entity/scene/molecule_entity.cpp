#include "app/ecs/entity/scene/molecule_entity.hpp"
#include "app/application/scene.hpp"
#include "app/ecs/component/aabb_component.hpp"
#include "app/ecs/component/transform_component.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/model/chemistry/molecule.hpp"
#include <renderer/gl/struct_proxy_molecule.hpp>

namespace VTX::App::ECS::Entity::Scene
{
	Core::BaseEntity generateMoleculeEntity( Application::Scene & p_scene )
	{
		ECS::Core::BaseEntity moleculeEntity = p_scene.createItem();

		ECS::MAIN_REGISTRY().addComponent<Model::Chemistry::Molecule>( moleculeEntity );
		ECS::MAIN_REGISTRY().addComponent<VTX::Renderer::GL::StructProxyMolecule>( moleculeEntity );
		ECS::MAIN_REGISTRY().addComponent<ECS::Component::AABB>( moleculeEntity );
		ECS::MAIN_REGISTRY().addComponent<ECS::Component::Transform>( moleculeEntity );

		return moleculeEntity;
	}
} // namespace VTX::App::ECS::Entity::Scene
