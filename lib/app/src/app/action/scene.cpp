#include "app/action/scene.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/events.hpp"
#include "app/scene/root.hpp"
#include "app/services.hpp"
#include "app/system/deleted.hpp"
#include "app/system/metadata.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
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
		auto & selection  = reg.emplace<System::Selection>( entity );
		auto & uid		  = reg.emplace<System::UID>( entity );
		auto & visibility = reg.emplace<System::Visibility>( entity );
		auto & deleted	  = reg.emplace<System::Deleted>( entity );

		// Load system.
		IO::Reader::System loader;
		// systemStruct.trajectory.setOptimized();

		if ( p_buffer ) // From buffer.
		{
			VTX_DEBUG( "Path: {}", p_path.string() );
			loader.readBuffer( *p_buffer, p_path, data );
		}
		else // From disk.
		{
			loader.readFile( p_path, data );
			metadata.path = p_path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metadata.pdbIDCode								   = pdbId;
		const std::string systemName					   = pdbId == "" ? p_path.stem().string() : pdbId;
		data.name										   = systemName; // TODO: move to metadata?

		// TODO: use uid manager.
		uid.system = System::UID::COUNTER++;

		// AABB (trigger update function for scene aabb).
		reg.patch<Util::Math::AABB>( entity, [ &loader ]( Util::Math::AABB & p_aabb ) { p_aabb = loader.getAABB(); } );

		// Orient.
		// auto [ _entScene, _root, sceneAABB ] = ECS::getFirstEntityWithComponents<App::Scene::Root,
		// Util::Math::AABB>();
		ACTION().execute<App::Action::Camera::Orient>( aabb );

		HUB().trigger<Events::SystemLoad>( { entity } );
	}

	void DeleteSystem::execute( const ECS::Entity p_entity ) { REG().destroy( p_entity ); }

	void Clear::execute()
	{
		REG().view<System::Metadata>().each( [ & ]( auto p_entity, auto & ) { REG().destroy( p_entity ); } );
	}

} // namespace VTX::App::Action::Scene
