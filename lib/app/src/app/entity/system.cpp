#include "app/component/chemistry/system.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/system_metadata.hpp"
#include "app/component/render/proxy_system.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/circular_buffer.hpp"
#include "app/core/player/players.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/entity/system.hpp"
#include "app/selection/system_data.hpp"
#include "app/selection/system_granularity.hpp"
#include "app/settings.hpp"
#include <io/reader/system.hpp>
#include <renderer/proxy/system.hpp>
#include <util/logger.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Entity
{
	void System::setup()
	{
		// TODO: share with wiewpoint entity.
		auto & sceneItemComponent = ECS_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		// Add components.
		auto & metaData	  = ECS_REGISTRY().addComponent<Component::IO::SystemMetadata>( *this );
		auto & system	  = ECS_REGISTRY().addComponent<Component::Chemistry::System>( *this );
		auto & aabb		  = ECS_REGISTRY().addComponent<Component::Scene::AABB>( *this );
		auto & transform  = ECS_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		auto & uid		  = ECS_REGISTRY().addComponent<Component::Scene::UIDComponent>( *this );
		auto & selectable = ECS_REGISTRY().addComponent<Component::Scene::Selectable>( *this );
		auto & pickable	  = ECS_REGISTRY().addComponent<Component::Scene::Pickable>( *this );
		auto & updatable  = ECS_REGISTRY().addComponent<Component::Scene::Updatable>( *this );
		auto & proxy	  = ECS_REGISTRY().addComponent<Component::Render::ProxySystem>( *this );

		// Load system.
		IO::Reader::System		  loader;
		VTX::Core::Struct::System systemStruct;

		if ( _buffer ) // From buffer.
		{
			VTX_DEBUG( "Path: {}", _path.string() );
			loader.readBuffer( *_buffer, _path, systemStruct );
		}
		else // From disk.
		{
			loader.readFile( _path, systemStruct );
			metaData.path = _path;
		}

		system.setSystemStruct( systemStruct );

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metaData.pdbIDCode								   = pdbId;

		system.setPdbIdCode( pdbId );

		const std::string systemName = pdbId == "" ? Util::Filesystem::getFileName( _path ) : pdbId;
		system.setName( systemName );

		// UID.
		uid.referenceUID( system.getAtomUIDs() );

		// Selectable.
		selectable.setSelectionDataGenerator<Selection::SystemData>();

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

		// Trajectory.
		if ( system.hasTrajectory() )
		{
			auto & trajectory = ECS_REGISTRY().addComponent<Component::Chemistry::Trajectory>( *this, &system, _path );

			if ( trajectory.getSystemPtr()->getTrajectory().IsOptimized() )
				trajectory.setPlayer(Util::Singleton<Core::Player::Players>::get().getOrCreate<Core::Player::CircularBuffer>());
			else
				trajectory.setPlayer(Util::Singleton<Core::Player::Players>::get().getOrCreate<Core::Player::Loop>());
		}

		// Proxy.
		proxy.setup( App::RENDERER_SYSTEM() );

		// Picking.
		pickable.setPickingFunction(
			[ & ]( const Selection::PickingInfo & p_pickingInfo )
			{
				const auto granularity
					= SETTINGS_SYSTEM().get<Selection::Granularity>( Settings::Selection::MOLECULE_GRANULARITY_KEY );

				std::unique_ptr<Selection::SelectionData> res = std::make_unique<Selection::SystemData>( selectable );
				auto &									  molData = dynamic_cast<Selection::SystemData &>( *res );
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
								Selection::SystemGranularity::getSelectionData( *atomPtr, granularity )
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
								Selection::SystemGranularity::getSelectionData( *residuePtr, granularity )
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
							molData.set( Selection::SystemGranularity::getSelectionData(
								*firstAtomPtr, granularity
							) );

							molData.add( Selection::SystemGranularity::getSelectionData(
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
