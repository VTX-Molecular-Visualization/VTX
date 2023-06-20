#include "app/ecs/entity/scene/molecule_entity.hpp"
#include "app/application/scene.hpp"
#include "app/ecs/component/aabb_component.hpp"
#include "app/ecs/component/transform_component.hpp"
#include "app/ecs/entity/scene/scene_item_entity.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/model/chemistry/molecule.hpp"
#include "app/old/event.hpp"
#include "app/old/event/global.hpp"
#include "app/render/proxy_builder.hpp"
#include <renderer/gl/struct_proxy_molecule.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::App::ECS::Entity::Scene
{
	void MoleculeEntityBuilder::addComponent( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::addComponent( p_entity, p_extraData );

		ECS::MAIN_REGISTRY().addComponent<Model::Chemistry::Molecule>( p_entity );
		ECS::MAIN_REGISTRY().addComponent<ECS::Component::AABB>( p_entity );
		ECS::MAIN_REGISTRY().addComponent<ECS::Component::Transform>( p_entity );
		ECS::MAIN_REGISTRY().addComponent<VTX::Renderer::GL::StructProxyMolecule>( p_entity );
	}
	void MoleculeEntityBuilder::setup( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::setup( p_entity, p_extraData );

		Model::Chemistry::Molecule & moleculeComponent
			= ECS::MAIN_REGISTRY().getComponent<Model::Chemistry::Molecule>( p_entity );

		_load( moleculeComponent, p_extraData );

		ECS::Component::SceneItemComponent & sceneComponent
			= ECS::MAIN_REGISTRY().getComponent<ECS::Component::SceneItemComponent>( p_entity );
		sceneComponent.setName( moleculeComponent.getPdbIdCode() );

		// Setup GPU Proxy
		Renderer::GL::StructProxyMolecule & gpuProxyComponent
			= ECS::MAIN_REGISTRY().getComponent<Renderer::GL::StructProxyMolecule>( p_entity );

		Render::GPUProxyBuilder::fillProxy( moleculeComponent, gpuProxyComponent );
	}

	void MoleculeEntityBuilder::_load( Model::Chemistry::Molecule & p_moleculeComponent,
									   const VariantMap &			p_extraData )
	{
		if ( p_extraData.find( "buffer" ) != p_extraData.end() )
		{
			const FilePath	  path	 = FilePath( p_extraData.at( "filepath" ).get<std::string>() );
			const std::string buffer = p_extraData.at( "buffer" ).get<std::string>();

			Internal::IO::Reader::MoleculeLoader libChemfiles = Internal::IO::Reader::MoleculeLoader();
			libChemfiles.readBuffer( buffer, path, p_moleculeComponent );
		}
		else if ( p_extraData.find( "filepath" ) != p_extraData.end() )
		{
			const FilePath path = FilePath( p_extraData.at( "filepath" ).get<std::string>() );

			Internal::IO::Reader::MoleculeLoader libChemfiles = Internal::IO::Reader::MoleculeLoader();
			libChemfiles.readFile( path, p_moleculeComponent );
		}
	}
} // namespace VTX::App::ECS::Entity::Scene
