#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/molecule_data.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/entity/scene/scene_item_entity.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/render/proxy_builder.hpp"
#include "app/vtx_app.hpp"
#include <renderer/struct_proxy_molecule.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

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
		MAIN_REGISTRY().addComponent<VTX::Renderer::StructProxyMolecule>( p_entity );
		MAIN_REGISTRY().addComponent<App::Component::Scene::Selectable>( p_entity );
	}
	void MoleculeEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::setup( p_entity, p_extraData );

		Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( p_entity );

		_load( moleculeComponent, p_extraData );

		Component::Scene::Selectable & selectableComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_entity );
		selectableComponent.setSelectionDataGenerator<Application::Selection::MoleculeData>();

		// Setup GPU Proxy
		Renderer::StructProxyMolecule & gpuProxyComponent
			= MAIN_REGISTRY().getComponent<Renderer::StructProxyMolecule>( p_entity );

		Render::GPUProxyBuilder::fillProxy( moleculeComponent, gpuProxyComponent );

		if ( moleculeComponent.hasTrajectory() )
		{
			MAIN_REGISTRY().addComponent<Component::Chemistry::Trajectory>( p_entity, &moleculeComponent );
		}
	}
	void MoleculeEntityBuilder::postSetup(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::postSetup( p_entity, p_extraData );
	}

	void MoleculeEntityBuilder::_load(
		Component::Chemistry::Molecule & p_moleculeComponent,
		const Util::VariantMap &		 p_extraData
	)
	{
		const Util::VariantMap::const_iterator filepathProperty = p_extraData.find( "filepath" );
		if ( filepathProperty == p_extraData.end() || !filepathProperty->second.is<std::string>() )
		{
			VTX_ERROR( "Missing property \"filepath\" in loading data." );
			return;
		}

		Internal::IO::Reader::MoleculeLoader loader = Internal::IO::Reader::MoleculeLoader();
		const FilePath						 path	= FilePath( filepathProperty->second.get<std::string>() );

		if ( p_extraData.find( "buffer" ) != p_extraData.end() )
		{
			const Util::VTXVariant buffer = p_extraData.at( "buffer" ).get<std::string>();

			if ( !buffer.is<std::string>() )
			{
				VTX_ERROR( "Invalid buffer." );
				return;
			}

			loader.readBuffer( buffer.get<std::string>(), path, p_moleculeComponent );
		}
		else // Filepath
		{
			loader.readFile( path, p_moleculeComponent );
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		p_moleculeComponent.setPdbIdCode( pdbId );

		const std::string moleculeName = pdbId == "" ? Util::Filesystem::getFileName( path ) : pdbId;
		p_moleculeComponent.setName( moleculeName );
	}
} // namespace VTX::App::Entity::Scene
