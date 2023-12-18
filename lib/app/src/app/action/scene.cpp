#include "app/action/scene.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/scene/molecule_entity.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Action::Scene
{
	void LoadMolecule::execute()
	{
		for ( const FilePath & moleculePath : _paths )
		{
			const std::unique_ptr<Application::ECS::Building::EntityBuilder> entityBuilder
				= VTXApp::get().getEntityDirector().generateBuilder( Entity::MOLECULE_ENTITY_ID );

			// Possibility to thread build function
			entityBuilder->getData()[ "scene" ]	   = Util::VTXVariant( &SCENE() );
			entityBuilder->getData()[ "filepath" ] = Util::VTXVariant( moleculePath.string() );
			entityBuilder->build();
		}
	}

} // namespace VTX::App::Action::Scene
