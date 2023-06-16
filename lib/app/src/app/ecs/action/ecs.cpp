#include "app/ecs/action/ecs.hpp"
#include "app/application/scene.hpp"
#include "app/ecs/entity/scene/molecule_entity.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/vtx_app.hpp"
#include <app/internal/io/reader/molecule_loader.hpp>
#include <app/render/proxy_builder.hpp>
#include <renderer/gl/struct_proxy_molecule.hpp>

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
				ECS::Core::BaseEntity moleculeEntity = ECS::Entity::Scene::generateMoleculeEntity( scene );

				// Load molecule
				Internal::IO::Reader::MoleculeLoader libChemfiles = Internal::IO::Reader::MoleculeLoader();
				Model::Chemistry::Molecule &		 moleculeComponent
					= ECS::MAIN_REGISTRY().getComponent<Model::Chemistry::Molecule>( moleculeEntity );

				libChemfiles.readFile( moleculePath, moleculeComponent );

				ECS::Component::SceneItemComponent & sceneComponent
					= ECS::MAIN_REGISTRY().getComponent<ECS::Component::SceneItemComponent>( moleculeEntity );
				sceneComponent.setName( moleculeComponent.getPdbIdCode() );

				// Setup GPU Proxy
				Renderer::GL::StructProxyMolecule & gpuProxyComponent
					= ECS::MAIN_REGISTRY().getComponent<Renderer::GL::StructProxyMolecule>( moleculeEntity );

				Render::GPUProxyBuilder::fillProxy( moleculeComponent, gpuProxyComponent );
			}
		}
	}

} // namespace VTX::App::ECS::Action
