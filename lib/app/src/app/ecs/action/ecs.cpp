#include "app/ecs/action/ecs.hpp"
#include "app/application/scene.hpp"
#include "app/ecs/building/entity_director.hpp"
#include "app/ecs/entity/scene/molecule_entity.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::ECS::Action
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

		//	LoadSceneClass * const sceneClass = new LoadSceneClass( _paths );
		//	sceneClass->_loadScene();

		//	delete sceneClass;
		//}
		// else
		{
			Application::Scene & scene = VTXApp::get().getScene();

			for ( const FilePath & moleculePath : _paths )
			{
				ECS::Building::EntityBuilder * const entityBuilder
					= ECS::Building::EntityDirector::generateBuilder( "Molecule" );

				// Possibility to thread build function
				entityBuilder->getData()[ "scene" ]	   = VTXVariant( &scene );
				entityBuilder->getData()[ "filepath" ] = VTXVariant( moleculePath.string() );
				entityBuilder->build();

				delete entityBuilder;
			}
		}
	}

} // namespace VTX::App::ECS::Action
