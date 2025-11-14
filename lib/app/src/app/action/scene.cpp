#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/system/metadata.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
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
		// Create entity.
		ECS::Entity system = REG().create();

		auto & metadata	  = REG().emplace<System::Metadata>( system );
		auto & trajectory = REG().emplace<System::Trajectory>( system );
		auto & transform  = REG().emplace<Util::Math::Transform>( system );
		auto & aabb		  = REG().emplace<Util::Math::AABB>( system );
		auto & selection  = REG().emplace<System::Selection>( system );
		auto & uid		  = REG().emplace<System::UID>( system );
		auto & data		  = REG().emplace<Core::Struct::System>( system ); // Last component to emplace.

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

		// TODO
		uid.system = System::UID::COUNTER++;

		// ACTION().execute<App::Action::Camera::Orient>( App::SCENE().getAABB() );

		HUB().trigger<Events::SystemLoad>();
	}

	void DeleteSystem::execute( const ECS::Entity p_entity ) { REG().destroy( p_entity ); }

	void Clear::execute()
	{
		REG().view<System::Metadata>().each( [ & ]( auto p_entity, auto & ) { REG().destroy( p_entity ); } );
	}

} // namespace VTX::App::Action::Scene
