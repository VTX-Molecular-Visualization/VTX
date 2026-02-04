#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/filesystem.hpp"
#include "app/network/network_manager.hpp"
#include "app/services.hpp"
#include "app/system/metadata.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include <io/reader/chemfiles.hpp>
#include <io/reader/system.hpp>
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::IO
{
	void Open::execute( const std::string & p_path ) { execute( FilePath( p_path ) ); }

	void Open::execute( const FilePath & p_path )
	{
		// TODO: check file format to redirect to the correct loader.
		auto view = REG().view<System::TrajectorySingleFrame, System::Metadata>();
		if ( std::distance( view.begin(), view.end() ) > 0
			 && VTX::IO::Reader::Chemfiles::isTrajectoryFileFormat( p_path ) )
			HUB().trigger<Events::TrajectoryFileAssociation>( Events::TrajectoryFileAssociation { p_path } );
		else
			ACTION().execute<Action::Scene::LoadSystem>( p_path );
	}
	void AssociateTrajectory::execute( const FilePath & p_path, const ECS::Entity & p_entity )
	{
		// TODO : Maybe there is some code to merge with the systemloading in scene.cpp
		size_t					initialAtomcount = REG().get<Core::Struct::System>( p_entity ).getAtomCount();
		VTX::IO::Reader::System loader;
		Core::Struct::System	data;
		loader.readFile( p_path, data );

		if ( initialAtomcount == data.getAtomCount() )
		{
			REG().remove<System::GenericTrajectory>( p_entity );
			auto & trajectory = REG().emplace<System::TrajectoryFullBuffer>( p_entity );
			auto & uid		  = REG().get<System::UID>( p_entity );
			App::System::prepare( p_entity, trajectory, std::move( loader ) );

			RENDERER().setSystemPosition( uid.system, trajectory.frameCollection[ 0 ] );
		}
		else
		{
			VTX::VTX_ERROR(
				"File {} and system {} has different atom count. ({}/{})",
				p_path.string(),
				REG().get<System::Metadata>( p_entity ).pdbIDCode,
				initialAtomcount,
				data.getAtomCount()
			);
		}
	}

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id )
	{
		execute( p_id, p_id.str + VTX::Util::Url::rcsbPdbDownloadFileExtension() );
	}

	void DownloadSystem::execute( const std::string & p_systemId )
	{
		execute( Util::Url::SystemId( p_systemId.data() ) );
	}

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id, FilePath p_path )
	{
		execute( VTX::Util::Url::UrlFull( p_id ), p_path );
	}

	void DownloadSystem::execute( VTX::Util::Url::UrlFull p_url, FilePath p_path )
	{
		FilePath filepath = p_path;
		NETWORK().downloadFile(
			p_url.str.data(),
			filepath.string(),
			[ filepath ]( const std::string & p_text )
			{ ACTION().execute<Action::Scene::LoadSystem>( filepath, &p_text ); }
		);
	}

	void Snapshot::execute()
	{
		execute(
			Filesystem::getSnapshotsDir() / std::to_string( Util::Chrono::getTimestamp() ),
			Util::Image::E_FORMAT::PNG,
			RENDERER().width(),
			RENDERER().height()
		);
	}

	void Snapshot::execute(
		const FilePath				p_path,
		const Util::Image::E_FORMAT p_format,
		const size_t				p_width,
		const size_t				p_height
	)
	{
		try
		{
			const auto &	   camera = ECS::getFirstComponent<Renderer::Camera>();
			std::vector<uchar> image;

			RENDERER().snapshot( image, _width, _height, camera.fov, camera.near, camera.far );
			FilePath path = Util::Image::write( _path, _format, _width, _height, image.data() );

			VTX_INFO( "Image saved: {}", path.string() );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Snapshot failed: {}", p_e.what() );
		}
	}
} // namespace VTX::App::Action::IO
