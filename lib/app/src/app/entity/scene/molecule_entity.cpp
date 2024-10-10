#include "app/entity/scene/molecule_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/molecule_data.hpp"
#include "app/application/selection/molecule_granularity.hpp"
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
#include "app/core/player/loop.hpp"
#include "app/core/player/players.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/serialization/io/reader/molecule_loader.hpp"
#include "app/settings.hpp"
#include <renderer/proxy/molecule.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Entity::Scene
{
	void MoleculeEntity::setup()
	{
		// TODO: share with wiewpoint entity.
		auto & sceneItemComponent = MAIN_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		// Add components.
		auto & molecule	  = MAIN_REGISTRY().addComponent<Component::Chemistry::Molecule>( *this );
		auto & aabb		  = MAIN_REGISTRY().addComponent<Component::Scene::AABB>( *this );
		auto & transform  = MAIN_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		auto & proxy	  = MAIN_REGISTRY().addComponent<Component::Render::ProxyMolecule>( *this );
		auto & uid		  = MAIN_REGISTRY().addComponent<Component::Scene::UIDComponent>( *this );
		auto & selectable = MAIN_REGISTRY().addComponent<Component::Scene::Selectable>( *this );
		auto & pickable	  = MAIN_REGISTRY().addComponent<Component::Scene::Pickable>( *this );

		// Load molecule.
		Serialization::IO::Reader::MoleculeLoader loader;
		auto & metaData = MAIN_REGISTRY().addComponent<Component::IO::MoleculeMetadata>( *this );

		if ( _buffer ) // Buffer.
		{
			VTX_DEBUG( "Path: {}", _path.string() );
			loader.readBuffer( *_buffer, _path, molecule );
		}
		else // Filepath
		{
			loader.readFile( _path, molecule );
			metaData.path = _path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metaData.pdbIDCode								   = pdbId;

		molecule.setPdbIdCode( pdbId );

		const std::string moleculeName = pdbId == "" ? Util::Filesystem::getFileName( _path ) : pdbId;
		molecule.setName( moleculeName );

		// UID.
		uid.referenceUID( molecule.getAtomUIDs() );
		// Selectable.
		selectable.setSelectionDataGenerator<Application::Selection::MoleculeData>();
		// AABB.
		aabb.init();
		aabb.setAABBComputationFunction(
			[ & ]()
			{
				Util::Math::AABB res;
				res.invalidate();

				for ( const auto & atomPtr : molecule.getAtoms() )
				{
					if ( atomPtr != nullptr )
						res.extend( atomPtr->getLocalPosition() );
				}

				return res;
			}
		);
		// Proxy.
		proxy.setup( App::RENDERER_SYSTEM() );

		// Trajectory.
		if ( molecule.hasTrajectory() )
		{
			auto & trajectory = MAIN_REGISTRY().addComponent<Component::Chemistry::Trajectory>( *this, &molecule );

			// TODO: set from settings.
			auto * const defaultPlayMode
				= Util::Singleton<Core::Player::Players>::get().getOrCreate<Core::Player::Loop>();

			trajectory.setPlayer( defaultPlayMode );
		}

		// Picking.
		pickable.setPickingFunction(
			[ & ]( const Application::Selection::PickingInfo & p_pickingInfo )
			{
				const auto granularity = SETTINGS_SYSTEM().get<Application::Selection::Granularity>(
					Settings::Selection::MOLECULE_GRANULARITY_KEY
				);

				std::unique_ptr<Application::Selection::SelectionData> res
					= std::make_unique<Application::Selection::MoleculeData>( selectable );
				auto & molData = dynamic_cast<Application::Selection::MoleculeData &>( *res );
				molData.clear();

				if ( p_pickingInfo.hasOneValue() )
				{
					// First UID is Atom and not the other one => Pick Atom
					if ( molecule.getAtomUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						const Component::Chemistry::Atom * const atomPtr
							= molecule.getAtomFromUID( p_pickingInfo.getFirst() );

						if ( atomPtr != nullptr )
						{
							molData.set(
								Application::Selection::MoleculeGranularity::getSelectionData( *atomPtr, granularity )
							);
						}
					}
					else if ( molecule.getResidueUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						// First UID is Residue => Pick Residue
						const Component::Chemistry::Residue * const residuePtr
							= molecule.getResidueFromUID( p_pickingInfo.getFirst() );

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
					if ( ( molecule.getAtomUIDs().contains( p_pickingInfo.getFirst() )
						   && molecule.getAtomUIDs().contains( p_pickingInfo.getSecond() ) ) )
					{
						const Component::Chemistry::Atom * const firstAtomPtr
							= molecule.getAtomFromUID( p_pickingInfo.getFirst() );
						const Component::Chemistry::Atom * const secondAtomPtr
							= molecule.getAtomFromUID( p_pickingInfo.getSecond() );

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

		auto & scene = SCENE();
		scene.referenceItem( sceneItemComponent );
	}
} // namespace VTX::App::Entity::Scene
