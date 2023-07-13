#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/entity/scene/scene_item_entity.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/render/proxy_builder.hpp"
#include <renderer/gl/struct_proxy_molecule.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Entity::Scene
{
	void MoleculeEntityBuilder::addComponent( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::addComponent( p_entity, p_extraData );

		MAIN_REGISTRY().addComponent<Component::Chemistry::Molecule>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::AABB>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( p_entity );
		MAIN_REGISTRY().addComponent<VTX::Renderer::GL::StructProxyMolecule>( p_entity );
	}
	void MoleculeEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::setup( p_entity, p_extraData );

		Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( p_entity );

		_load( moleculeComponent, p_extraData );

		Component::Scene::SceneItemComponent & sceneComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( p_entity );
		sceneComponent.setName( moleculeComponent.getPdbIdCode() );

		// Setup GPU Proxy
		Renderer::GL::StructProxyMolecule & gpuProxyComponent
			= MAIN_REGISTRY().getComponent<Renderer::GL::StructProxyMolecule>( p_entity );

		Render::GPUProxyBuilder::fillProxy( moleculeComponent, gpuProxyComponent );
	}
	void MoleculeEntityBuilder::postSetup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::postSetup( p_entity, p_extraData );
	}

	void MoleculeEntityBuilder::_load( Component::Chemistry::Molecule & p_moleculeComponent,
									   const VariantMap &				p_extraData )
	{
		Internal::IO::Reader::MoleculeLoader loader = Internal::IO::Reader::MoleculeLoader();
		const FilePath						 path	= FilePath( p_extraData.at( "filepath" ).get<std::string>() );

		if ( p_extraData.find( "buffer" ) != p_extraData.end() )
		{
			const std::string buffer = p_extraData.at( "buffer" ).get<std::string>();
			loader.readBuffer( buffer, path, p_moleculeComponent );
		}
		else if ( p_extraData.find( "filepath" ) != p_extraData.end() )
		{
			loader.readFile( path, p_moleculeComponent );
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		p_moleculeComponent.setPdbIdCode( chemfilesReader.getPdbIdCode() );
	}
} // namespace VTX::App::Entity::Scene