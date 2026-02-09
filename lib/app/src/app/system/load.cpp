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
#include "app/system/load.hpp"
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
	namespace
	{

		template<typename TrajectoryType>
		void createTrajectory( const ECS::Entity & p_entity, TrajectoryType && p_traj )
		{
		}

	} // namespace

	void create( const ECS::Entity & p_entity, PendingSystem & p_data ) noexcept
	{
		auto & reg = REG();

		// Add components.
		auto & data		 = reg.emplace<Core::Struct::System>( p_entity );
		auto & metadata	 = reg.emplace<System::Metadata>( p_entity );
		auto & transform = reg.emplace<Util::Math::Transform>( p_entity );
		auto & aabb		 = reg.emplace<Util::Math::AABB>( p_entity );
		auto & uid		 = reg.emplace<System::UID>( p_entity );

		auto & visibility	  = reg.emplace<System::Visibility>( p_entity );
		auto & selection	  = reg.emplace<System::Selection>( p_entity );
		auto & representation = reg.emplace<System::Representation>( p_entity );
		auto & color		  = reg.emplace<System::Color>( p_entity );
		auto & deleted		  = reg.emplace<System::Deleted>( p_entity );

		std::visit(
			[ &p_entity ]( auto && traj ) mutable
			{
				using TrajType = std::remove_cvref_t<decltype( traj )>;
				TrajType & t   = REG().emplace<TrajType>( p_entity );
				t			   = std::move( traj );
			},
			std::move( p_data.trajectoryData )
		);

		const std::string & pdbId	 = p_data.pdbIdCode;
		metadata.pdbIDCode			 = pdbId;
		const std::string systemName = pdbId == "" ? p_data.path.stem().string() : pdbId;
		data.name					 = systemName; // TODO: move to metadata?

		// UIDs: get from UID manager.
		auto & uidManager = App::UID();
		uid.system		  = uidManager.getRootPool().registerValue();
		uid.residues	  = uidManager.getPickingPool().registerRange( data.getResidueCount() );
		uid.atoms		  = uidManager.getPickingPool().registerRange( data.getAtomCount() );

		std::span<const Vec3f> firstFrame = getCurrentAtomPositions( p_entity );
		RENDERER().setSystemPosition( uid.system, firstFrame );

		// AABB (trigger update function for scene aabb).
		reg.patch<Util::Math::AABB>(
			p_entity,
			[ &firstFrame ]( Util::Math::AABB & p_aabb )
			{
				for ( auto & it_atomPos : firstFrame )
				{
					p_aabb.extend( it_atomPos, Core::ChemDB::Atom::VDW_RADIUS_MIN );
				}
			}
		);

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

		reg.erase<PendingSystem>( p_entity );
	}

} // namespace VTX::App::System
