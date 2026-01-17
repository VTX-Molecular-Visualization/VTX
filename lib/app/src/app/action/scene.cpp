#include "app/action/scene.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/action/color_scheme.hpp"
#include "app/action/visibility.hpp"
#include "app/events.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/deleted.hpp"
#include "app/system/metadata.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Action::Scene
{

	void LoadSystem::execute( const FilePath & p_path, const std::string * const p_buffer )
	{
		auto & reg = REG();

		// Create entity.
		ECS::Entity entity = reg.create();

		// Add components.
		auto & data		  = reg.emplace<Core::Struct::System>( entity );
		auto & metadata	  = reg.emplace<System::Metadata>( entity );
		auto & trajectory = reg.emplace<System::Trajectory>( entity );
		auto & transform  = reg.emplace<Util::Math::Transform>( entity );
		auto & aabb		  = reg.emplace<Util::Math::AABB>( entity );
		auto & uid		  = reg.emplace<System::UID>( entity );

		auto & visibility	  = reg.emplace<System::Visibility>( entity );
		auto & selection	  = reg.emplace<System::Selection>( entity );
		auto & representation = reg.emplace<System::Representation>( entity );
		auto & color		  = reg.emplace<System::Color>( entity );
		auto & deleted		  = reg.emplace<System::Deleted>( entity );

		// Load system data and metadata.
		IO::Reader::System loader;
		// systemStruct.trajectory.setOptimized();

		// From buffer.
		if ( p_buffer )
		{
			VTX_DEBUG( "Path: {}", p_path.string() );
			loader.readBuffer( *p_buffer, p_path, data );
		}
		// From disk.
		else
		{
			metadata.path = p_path;
			loader.readFile( p_path, data );
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metadata.pdbIDCode								   = pdbId;
		const std::string systemName					   = pdbId == "" ? p_path.stem().string() : pdbId;
		data.name										   = systemName; // TODO: move to metadata?

		// AABB (trigger update function for scene aabb).
		reg.patch<Util::Math::AABB>( entity, [ &loader ]( Util::Math::AABB & p_aabb ) { p_aabb = loader.getAABB(); } );

		// UIDs: get from UID manager.
		auto & uidManager = UID();
		uid.system		  = uidManager.getRootPool().registerValue();
		uid.residues	  = uidManager.getPickingPool().registerRange( data.getResidueCount() );
		uid.atoms		  = uidManager.getPickingPool().registerRange( data.getAtomCount() );

		// Color: set size.
		color.atoms.resize( data.getAtomCount() );

		// Visibillity: set default all visible.
		ACTION().execute<Visibility::SetVisible<App::Scene::E_ITEM::SYSTEM>>( entity );

		// Selection : Nothing to do.

		// Representation.
		// TODO: apply default representation preset.

		// Color scheme.
		ACTION().execute<ColorScheme::Add<System::E_COLOR_SCHEME::ATOM>>( entity );

		// Deleted: nothing to do.

		// Orient.
		ACTION().execute<App::Action::Camera::Orient>( aabb );

		// Trigger system load.
		HUB().trigger<Events::SystemLoad>( { entity } );
	}

	void DeleteSystem::execute( const ECS::Entity p_entity ) { REG().destroy( p_entity ); }

	void Clear::execute()
	{
		REG().view<System::Metadata>().each( [ & ]( auto p_entity, auto & ) { REG().destroy( p_entity ); } );
	}

} // namespace VTX::App::Action::Scene
