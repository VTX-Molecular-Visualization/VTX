#include "app/component/chemistry/system.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/system_data.hpp"
#include "app/application/selection/system_granularity.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/system_metadata.hpp"
#include "app/component/render/proxy_system.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/players.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/entity/system.hpp"
#include "app/serialization/io/reader/system_loader.hpp"
#include "app/settings.hpp"
#include <renderer/proxy/system.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Entity
{
	void System::setup()
	{
		// TODO: share with wiewpoint entity.
		auto & sceneItemComponent = ECS_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		// Add components.
		auto & system	  = ECS_REGISTRY().addComponent<Component::Chemistry::System>( *this );
		auto & aabb		  = ECS_REGISTRY().addComponent<Component::Scene::AABB>( *this );
		auto & transform  = ECS_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		auto & proxy	  = ECS_REGISTRY().addComponent<Component::Render::ProxySystem>( *this );
		auto & uid		  = ECS_REGISTRY().addComponent<Component::Scene::UIDComponent>( *this );
		auto & selectable = ECS_REGISTRY().addComponent<Component::Scene::Selectable>( *this );
		auto & pickable	  = ECS_REGISTRY().addComponent<Component::Scene::Pickable>( *this );

		// Load system.
		Serialization::IO::Reader::SystemLoader loader;
		auto & metaData = ECS_REGISTRY().addComponent<Component::IO::SystemMetadata>( *this );

		if ( _buffer ) // Buffer.
		{
			VTX_DEBUG( "Path: {}", _path.string() );
			loader.readBuffer( *_buffer, _path, system );
		}
		else // Filepath
		{
			loader.readFile( _path, system );
			metaData.path = _path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metaData.pdbIDCode								   = pdbId;

		system.setPdbIdCode( pdbId );

		const std::string systemName = pdbId == "" ? Util::Filesystem::getFileName( _path ) : pdbId;
		system.setName( systemName );

		// UID.
		uid.referenceUID( system.getAtomUIDs() );

		// Selectable.
		selectable.setSelectionDataGenerator<Application::Selection::SystemData>();

		// AABB.
		aabb.init();
		aabb.setAABBComputationFunction(
			[ & ]()
			{
				Util::Math::AABB res;
				res.invalidate();

				for ( const auto & atomPtr : system.getAtoms() )
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
		if ( system.hasTrajectory() )
		{
			auto & trajectory = ECS_REGISTRY().addComponent<Component::Chemistry::Trajectory>( *this, &system );

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
					= std::make_unique<Application::Selection::SystemData>( selectable );
				auto & molData = dynamic_cast<Application::Selection::SystemData &>( *res );
				molData.clear();

				bool test = system.getResidueUIDs().contains( p_pickingInfo.getFirst() );

				if ( p_pickingInfo.hasOneValue() )
				{
					// First UID is Atom and not the other one => Pick Atom
					if ( system.getAtomUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						const Component::Chemistry::Atom * const atomPtr
							= system.getAtomFromUID( p_pickingInfo.getFirst() );

						if ( atomPtr != nullptr )
						{
							molData.set(
								Application::Selection::SystemGranularity::getSelectionData( *atomPtr, granularity )
							);
						}
					}
					else if ( system.getResidueUIDs().contains( p_pickingInfo.getFirst() ) )
					{
						// First UID is Residue => Pick Residue
						const Component::Chemistry::Residue * const residuePtr
							= system.getResidueFromUID( p_pickingInfo.getFirst() );

						if ( residuePtr != nullptr )
						{
							molData.set(
								Application::Selection::SystemGranularity::getSelectionData( *residuePtr, granularity )
							);
						}
					}
				}
				else if ( p_pickingInfo.hasTwoValues() )
				{
					// Two atoms picked => Pick Bond
					if ( ( system.getAtomUIDs().contains( p_pickingInfo.getFirst() )
						   && system.getAtomUIDs().contains( p_pickingInfo.getSecond() ) ) )
					{
						const Component::Chemistry::Atom * const firstAtomPtr
							= system.getAtomFromUID( p_pickingInfo.getFirst() );
						const Component::Chemistry::Atom * const secondAtomPtr
							= system.getAtomFromUID( p_pickingInfo.getSecond() );

						if ( firstAtomPtr != nullptr && secondAtomPtr != nullptr )
						{
							molData.set( Application::Selection::SystemGranularity::getSelectionData(
								*firstAtomPtr, granularity
							) );

							molData.add( Application::Selection::SystemGranularity::getSelectionData(
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
} // namespace VTX::App::Entity
