#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/application.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/network/network_manager.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include "app/system/load.hpp"
#include "app/system/trajectory_preparation.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
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
			ACTION().execute<LoadSystem>( p_path );
	}
	struct _SystemIo
	{
		Util::StopToken						   stopToken;
		Threading::OptionalThreadReference	   threadRef;
		std::latch							   extractorCreation { 1 };
		std::optional<System::SystemExtractor> extractor;
		inline void							   wait() noexcept
		{
			this->extractorCreation.wait();
			if ( this->extractor )
				this->extractor->wait();
		}

		inline void start_extraction()
		{
			assert( extractor );
			extractor.value()( stopToken, threadRef );
		}
	};
	void _SystemIoDel::operator()( _SystemIo * p_ ) noexcept { delete p_; }

	LoadSystem::LoadSystem() : _data( new _SystemIo() ) {}
	LoadSystem::LoadSystem( Util::StopToken p_token, Threading::OptionalThreadReference p_thr ) :
		_data( new _SystemIo { std::move( p_token ), std::move( p_thr ) } )
	{
	}

	void LoadSystem::execute( FilePath p_path )
	{
		_data->extractor = System::SystemExtractor( std::move( p_path ) );
		_data->extractorCreation.count_down();

		_data->start_extraction();
	}
	void LoadSystem::execute( FilePath p_path, std::string && p_buffer )
	{
		_data->extractor = System::SystemExtractor( std::move( p_path ), std::move( p_buffer ) );
		_data->extractorCreation.count_down();

		_data->start_extraction();
	}
	void LoadSystem::wait() noexcept { _data->wait(); }

	AssociateTrajectory::AssociateTrajectory() : _data( new _SystemIo() ) {}
	AssociateTrajectory::AssociateTrajectory( Util::StopToken p_token, Threading::OptionalThreadReference p_thr ) :
		_data( new _SystemIo { std::move( p_token ), std::move( p_thr ) } )

	{
	}
	void AssociateTrajectory::execute( const FilePath & p_path, const ECS::Entity & p_entity )
	{
		if ( p_entity == entt::null )
		{
			VTX_ERROR( "System entity null." );
			return;
		}

		_data->extractor = System::SystemExtractor( p_entity, p_path );
		_data->extractorCreation.count_down();
		_data->start_extraction();
	}
	void AssociateTrajectory::execute( const std::string & p_path, const ECS::Entity & p_e )
	{ execute( FilePath( p_path ), p_e ); }
	void AssociateTrajectory::wait() noexcept { _data->wait(); }

	void RunPythonScript::execute( const FilePath & p_path ) { INTERPRETOR().runScript( p_path ); }

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id )
	{ execute( p_id, p_id.str + VTX::Util::Url::rcsbPdbDownloadFileExtension() ); }

	void DownloadSystem::execute( const std::string & p_systemId )
	{ execute( Util::Url::SystemId( p_systemId.data() ) ); }

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id, FilePath p_path )
	{ execute( VTX::Util::Url::UrlFull( p_id ), p_path ); }

	void DownloadSystem::execute( VTX::Util::Url::UrlFull p_url, FilePath p_path )
	{
		FilePath filepath = p_path;
		NETWORK().downloadFile(
			p_url.str.data(),
			filepath.string(),
			[ filepath ]( std::string && p_text )
			{ ACTION().execute<Action::IO::LoadSystem>( filepath, std::move( p_text ) ); }
		);
	}

	void Snapshot::execute()
	{
		execute(
			SESSION().getSnapshotsDir() / std::to_string( Util::Chrono::getTimestamp() ),
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
			const size_t currentWidth  = RENDERER().width();
			const size_t currentHeight = RENDERER().height();

			ACTION().execute<Application::Resize>( p_width, p_height );
			std::vector<std::byte> image = RENDERER().snapshot();
			ACTION().execute<Application::Resize>( currentWidth, currentHeight );

			FilePath path = Util::Image::write( p_path, p_format, p_width, p_height, image.data() );

			VTX_INFO( "Image saved: {}", path.string() );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Snapshot failed: {}", p_e.what() );
		}
	}
} // namespace VTX::App::Action::IO
