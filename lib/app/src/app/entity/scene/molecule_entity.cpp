#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/behaviour/molecule_behaviour.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include "app/component/render/proxy_molecule.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/uid_component.hpp"
#include <renderer/proxy/molecule.hpp>

namespace VTX::App::Entity::Scene
{
	void MoleculeEntity::setup()
	{
		// TODO: share with wiewpoint entity.
		auto & sceneItemComponent = MAIN_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		MAIN_REGISTRY().addComponent<Component::Chemistry::Molecule>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::AABB>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		MAIN_REGISTRY().addComponent<Component::Render::ProxyMolecule>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::UIDComponent>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::Selectable>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::Pickable>( *this );
		auto & moleculeBehaviour = MAIN_REGISTRY().addComponent<Component::Behaviour::Molecule>( *this );

		moleculeBehaviour.init( _path, _buffer );

		auto & scene = SCENE();
		scene.referenceItem( sceneItemComponent );
	}
} // namespace VTX::App::Entity::Scene
