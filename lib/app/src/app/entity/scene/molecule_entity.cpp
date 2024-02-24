#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/molecule_data.hpp"
#include "app/application/selection/molecule_granularity.hpp"
#include "app/application/settings.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/core/trajectory_player/base_player.hpp"
#include "app/core/trajectory_player/loop.hpp"
#include "app/core/trajectory_player/players.hpp"
#include "app/entity/scene/scene_item_entity.hpp"
#include "app/internal/application/settings.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/render/proxy_builder.hpp"
#include "app/vtx_app.hpp"
#include <renderer/facade.hpp>
#include <renderer/proxy/molecule.hpp>
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
		// MAIN_REGISTRY().addComponent<VTX::Renderer::Proxy::Molecule>( p_entity );
		MAIN_REGISTRY().addComponent<App::Component::Scene::UIDComponent>( p_entity );
		MAIN_REGISTRY().addComponent<App::Component::Scene::Selectable>( p_entity );
		MAIN_REGISTRY().addComponent<App::Component::Scene::Pickable>( p_entity );
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

		Component::Scene::UIDComponent & uidComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::UIDComponent>( p_entity );
		uidComponent.referenceUID( moleculeComponent.getAtomUIDs() );

		// Setup GPU Proxy
		// Renderer::Proxy::Molecule & gpuProxyComponent
		//	= MAIN_REGISTRY().getComponent<Renderer::Proxy::Molecule>( p_entity );

		// Render::GPUProxyBuilder::fillProxy( moleculeComponent, gpuProxyComponent );

		if ( moleculeComponent.hasTrajectory() )
		{
			Component::Chemistry::Trajectory & trajectoryComponent
				= MAIN_REGISTRY().addComponent<Component::Chemistry::Trajectory>( p_entity, &moleculeComponent );

			std::unique_ptr<App::Core::TrajectoryPlayer::BasePlayer> defaultPlayMode
				= App::Core::TrajectoryPlayer::Players::get().instantiateItem<App::Core::TrajectoryPlayer::Loop>(
					App::Core::TrajectoryPlayer::Loop::COLLECTION_ID
				);

			trajectoryComponent.setPlayer( defaultPlayMode );
		}

		Component::Scene::Pickable & pickableComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Pickable>( p_entity );

		pickableComponent.setPickingFunction(
			[ p_entity ]( const Application::Selection::PickingInfo & p_pickingInfo )
			{
				const Component::Chemistry::Molecule & moleculeComponent
					= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( p_entity );
				const Component::Scene::Selectable & selectableComponent
					= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_entity );

				const Application::Selection::Granularity granularity
					= SETTINGS().get<Application::Selection::Granularity>(
						Internal::Application::Settings::Selection::MOLECULE_GRANULARITY_KEY
					);

				std::unique_ptr<Application::Selection::SelectionData> res
					= std::make_unique<Application::Selection::MoleculeData>( selectableComponent );

				Application::Selection::MoleculeData & molData
					= dynamic_cast<Application::Selection::MoleculeData &>( *res );

				if ( p_pickingInfo.hasOneValue() )
				{
					// First UID is Atom and not the other one => Pick Atom
					if ( moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						const Component::Chemistry::Atom * const atomPtr
							= moleculeComponent.getAtomFromUID( p_pickingInfo.getFirst() );

						if ( atomPtr != nullptr )
						{
							molData.set(
								Application::Selection::MoleculeGranularity::getSelectionData( *atomPtr, granularity )
							);
						}
					}
					else if ( moleculeComponent.getResidueUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						// First UID is Residue => Pick Residue
						const Component::Chemistry::Residue * const residuePtr
							= moleculeComponent.getResidueFromUID( p_pickingInfo.getFirst() );

						if ( residuePtr != nullptr )
						{
							molData.set( Application::Selection::MoleculeGranularity::getSelectionData(
								*residuePtr, granularity
							) );
						}
					}
				}
				else if ( p_pickingInfo.hasTwoValues() )
				{
					// Two atoms picked => Pick Bond
					if ( ( moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getFirst() )
						   && moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getSecond() ) ) )
					{
						const Component::Chemistry::Atom * const firstAtomPtr
							= moleculeComponent.getAtomFromUID( p_pickingInfo.getFirst() );
						const Component::Chemistry::Atom * const secondAtomPtr
							= moleculeComponent.getAtomFromUID( p_pickingInfo.getSecond() );

						if ( firstAtomPtr != nullptr && secondAtomPtr != nullptr )
						{
							molData.set( Application::Selection::MoleculeGranularity::getSelectionData(
								*firstAtomPtr, granularity
							) );

							molData.add( Application::Selection::MoleculeGranularity::getSelectionData(
								*secondAtomPtr, granularity
							) );
						}
					}
				}

				return res;
			}
		);
	}
	void MoleculeEntityBuilder::postSetup(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::postSetup( p_entity, p_extraData );
		RENDERER().addProxy( MAIN_REGISTRY().getComponent<Renderer::Proxy::Molecule>( p_entity ) );
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

		const Core::ECS::BaseEntity &	  entity = MAIN_REGISTRY().getEntity( p_moleculeComponent );
		Component::IO::MoleculeMetadata & metaData
			= MAIN_REGISTRY().addComponent<Component::IO::MoleculeMetadata>( entity );

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
			metaData.path = path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metaData.pdbIDCode								   = pdbId;

		p_moleculeComponent.setPdbIdCode( pdbId );

		const std::string moleculeName = pdbId == "" ? Util::Filesystem::getFileName( path ) : pdbId;
		p_moleculeComponent.setName( moleculeName );
	}
} // namespace VTX::App::Entity::Scene
