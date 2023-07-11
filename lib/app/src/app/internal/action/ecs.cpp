#include "app/internal/action/ecs.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/scene/molecule_entity.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::Action::ECS
{
	void Open::execute()
	{
		// bool loadScene = false;
		// for ( const FilePath & path : _paths )
		//{
		//	loadScene = loadScene || path.extension() == "vtx";
		// }

		// if ( loadScene )
		//{
		//	if ( _paths.empty() )
		//		return;

		//	App::VTXApp::get().getScene().clear();

		//	const std::unique_ptr<LoadSceneClass> sceneClass = std::make_unique<LoadSceneClass>( _paths );
		//	sceneClass->_loadScene();
		//}
		// else
		{
			Application::Scene & scene = VTXApp::get().getScene();

			for ( const FilePath & moleculePath : _paths )
			{
				const std::unique_ptr<Application::ECS::Building::EntityBuilder> entityBuilder
					= Application::ECS::EntityDirector::generateBuilder( Entity::MOLECULE_ENTITY_ID );

				// Possibility to thread build function
				entityBuilder->getData()[ "scene" ]	   = VTXVariant( &scene );
				entityBuilder->getData()[ "filepath" ] = VTXVariant( moleculePath.string() );
				entityBuilder->build();
			}
		}
	}

	void OpenFlat::execute()
	{
		Application::Scene & scene = VTXApp::get().getScene();

		for ( const FilePath & moleculePath : _paths )
		{
			const std::unique_ptr<Application::ECS::Building::EntityBuilder> entityBuilder
				= Application::ECS::EntityDirector::generateBuilder( Entity::FLAT_MOLECULE_ENTITY_ID );

			// Possibility to thread build function
			entityBuilder->getData()[ "scene" ]	   = VTXVariant( &scene );
			entityBuilder->getData()[ "filepath" ] = VTXVariant( moleculePath.string() );
			entityBuilder->build();
		}
	}

} // namespace VTX::App::Internal::Action::ECS
