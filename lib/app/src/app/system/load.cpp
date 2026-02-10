#include "app/system/load.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/action/color.hpp"
#include "app/action/representation.hpp"
#include "app/action/scene.hpp"
#include "app/action/visibility.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/preset/name.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/deleted.hpp"
#include "app/system/metadata.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory_preparation.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/chemdb/atom.hpp>
#include <core/struct/system.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::System
{

	std::function<uint( Util::StopToken, Threading::BaseThread & )> fillerCallable(
		const ECS::Entity & p_entity,
		FilePath			p_path,
		PendingSystem &		p_pendingData
	) noexcept
	{
		return [ p_entity,
				 path = std::move( p_path ),
				 &p_pendingData ]( VTX::Util::StopToken p_stopToken, Threading::BaseThread & ) -> uint
		{
			p_pendingData.path = std::move( path );
			p_pendingData.loader.emplace();
			p_pendingData.loader->readFile( path, p_pendingData.system );
			p_pendingData.pdbIdCode = p_pendingData.loader->getChemfilesReader().getPdbIdCode();

			if ( p_stopToken.stop_requested() )
				return 0;

			p_pendingData.topologyReady = true;
			p_pendingData.trajectoryDecision.wait();

			if ( p_stopToken.stop_requested() )
				return 0;

			auto visitor = [ loader = &p_pendingData.loader.value() ]( auto && traj )
			{ System::prepare( traj, std::move( *loader ) ); };
			std::visit( visitor, p_pendingData.trajectoryData );
			p_pendingData.trajectoryReady = true;
			return 0;
		};
	}

	void addTrajectory( const ECS::Entity & p_entity, PendingSystem & p_data ) noexcept
	{
		std::visit(
			[ &p_entity ]( auto && traj ) mutable
			{
				using TrajType = std::remove_cvref_t<decltype( traj )>;
				TrajType & t   = REG().emplace<TrajType>( p_entity );
				t			   = std::move( traj );
			},
			std::move( p_data.trajectoryData )
		);
		startAsyncWork( p_entity, p_data );
		std::span<const Vec3f> firstFrame = getCurrentAtomPositions( p_entity );

		if ( auto uid = REG().try_get<System::UID>( p_entity ) )
			RENDERER().setSystemPosition( uid->system, firstFrame );

		// AABB (trigger update function for scene aabb).
		REG().patch<Util::Math::AABB>(
			p_entity,
			[ &firstFrame ]( Util::Math::AABB & p_aabb )
			{
				for ( auto & it_atomPos : firstFrame )
				{
					p_aabb.extend( it_atomPos, Core::ChemDB::Atom::VDW_RADIUS_MIN );
				}
			}
		);
	}
	void create( const ECS::Entity & p_entity, PendingSystem & p_data ) noexcept
	{
		auto & reg = REG();

		// Add components.
		auto & data		 = reg.emplace<Core::Struct::System>( p_entity, std::move( p_data.system ) );
		auto & metadata	 = reg.emplace<System::Metadata>( p_entity );
		auto & transform = reg.emplace<Util::Math::Transform>( p_entity );
		auto & aabb		 = reg.emplace<Util::Math::AABB>( p_entity );
		auto & uid		 = reg.emplace<System::UID>( p_entity );

		auto & visibility	  = reg.emplace<System::Visibility>( p_entity );
		auto & selection	  = reg.emplace<System::Selection>( p_entity );
		auto & representation = reg.emplace<System::Representation>( p_entity );
		auto & color		  = reg.emplace<System::Color>( p_entity );
		auto & deleted		  = reg.emplace<System::Deleted>( p_entity );

		const std::string & pdbId	 = p_data.pdbIdCode;
		metadata.pdbIDCode			 = pdbId;
		const std::string systemName = pdbId == "" ? p_data.path.stem().string() : pdbId;
		metadata.name				 = systemName;
		data.name					 = systemName; // TODO: remove

		// UIDs: get from UID manager.
		auto & uidManager = App::UID();
		uid.system		  = uidManager.getRootPool().registerValue();
		uid.residues	  = uidManager.getPickingPool().registerRange( data.getResidueCount() );
		uid.atoms		  = uidManager.getPickingPool().registerRange( data.getAtomCount() );

		addTrajectory( p_entity, p_data );

		// Visibillity: all visible.
		visibility.atoms = Core::Struct::IndexRangeList( data.getAtomRange() );

		// Selection: nothing selected.
		selection.atoms = {};

		// Deleted: nothing deleted.
		deleted.atoms = {};

		// Color: set default color scheme.
		color.colorSchemeAtoms[ System::E_COLOR_SCHEME::ATOM ] = Core::Struct::IndexRangeList( data.getAtomRange() );

		// Representation: set default representation.
		// TODO: configure default representation in settings?
		representation.presetAtoms[ ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::Representation>() ]
			= Core::Struct::IndexRangeList( data.getAtomRange() );

		// Trigger system load.
		HUB().trigger<Events::SystemLoad>( { p_entity } );

		// Orient.
		ACTION().execute<Action::Camera::Orient>( aabb );
	}

	void deliver( const ECS::Entity & p_entity, PendingSystem & p_data ) noexcept
	{
		auto system = REG().try_get<Core::Struct::System>( p_entity );
		if ( p_data.onlyTrajectory && system )
		{
			if ( system->getAtomCount() == p_data.system.getAtomCount() )
			{
				addTrajectory( p_entity, p_data );
			}
			else
			{
				VTX::VTX_ERROR(
					"File {} and system {} has different atom count. ({}/{})",
					p_data.path.string(),
					system->name,
					system->getAtomCount(),
					p_data.system.getAtomCount()
				);
			}
		}
		else
			create( p_entity, p_data );

		REG().erase<PendingSystem>( p_entity );
	}

} // namespace VTX::App::System
