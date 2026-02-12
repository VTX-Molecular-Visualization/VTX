#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/filesystem.hpp"
#include "app/network/network_manager.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include "app/system/load.hpp"
#include "app/system/metadata.hpp"
#include "app/system/trajectory_preparation.hpp"
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
		std::string extension = p_path.extension().string();
		if ( extension == ".py" || extension == ".vtx" )
			ACTION().execute<RunPythonScript>( p_path );
		else
			ACTION().execute<Action::Scene::LoadSystem>( p_path );
	}

	struct AssociateTrajectory::_Data
	{
		std::latch							   extractorCreation { 1 };
		std::optional<System::SystemExtractor> extractor;
	};
	void AssociateTrajectory::Del::operator()( AssociateTrajectory::_Data * p_ ) noexcept { delete p_; }

	AssociateTrajectory::AssociateTrajectory() : _data( new _Data() ) {}
	void AssociateTrajectory::execute( const FilePath & p_path, const ECS::Entity & p_entity )
	{
		if ( p_entity == entt::null )
		{
			VTX_ERROR( "System entity null." );
			return;
		}

		auto & pendingSystemData		 = REG().emplace<System::PendingSystem>( p_entity );
		pendingSystemData.onlyTrajectory = true;
		pendingSystemData.path			 = p_path;

		_data->extractor = System::SystemExtractor( p_entity, pendingSystemData );
		_data->extractorCreation.count_down();

		THREAD().createThread( _data->extractor.value() );
	}
	void AssociateTrajectory::execute( const std::string & p_path, const ECS::Entity & p_e )
	{
		execute( FilePath( p_path ), p_e );
	}
	void AssociateTrajectory::wait() noexcept
	{
		if ( _data == nullptr )
			return;
		_data->extractorCreation.wait();
		if ( _data->extractor )
			_data->extractor->wait();
	}

	void RunPythonScript::execute( const FilePath & p_path ) { INTERPRETOR().runScript( p_path ); }

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
			[ filepath ]( std::string && p_text )
			{ ACTION().execute<Action::Scene::LoadSystem>( filepath, std::move( p_text ) ); }
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
