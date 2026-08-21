#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/constants.hpp"
#include "app/extractor/mesh.hpp"
#include "app/extractor/system.hpp"
#include "app/network/network_manager.hpp"
#if VTX_PYTHON_BINDING
#include "app/python_binding/interpretor.hpp"
#endif
#include "app/arguments.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include "app/system/uid.hpp"
#include "app/system/writer.hpp"
#include <fmt/format.h>
#include <io/mesh_reader.hpp>
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <string>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/resolution.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Action::IO
{
	namespace
	{
		void _writeSnapshotMetadata( const FilePath & p_path )
		{
			try
			{
				Util::Image::writeSoftwareMetadata( p_path, APPLICATION_NAME );
			}
			catch ( const std::exception & p_e )
			{
				VTX_WARNING( "Unable to write image metadata: {}", p_e.what() );
			}
		}
	} // namespace

	void Open::execute( const std::string & p_path ) { execute( FilePath( p_path ) ); }

	void Open::execute( const FilePath & p_path )
	{
		const std::string extension = p_path.extension().string();
		if ( extension == ".py" || extension == ".vtx" )
		{
			if ( ARGS().noPython )
			{
				VTX_WARNING( "Python is disabled. VTX will skip script <{}>.", p_path.string() );
				return;
			}

			ACTION().execute<RunPythonScript>( p_path );
		}
		else if ( VTX::IO::isMeshFileFormat( p_path ) )
		{
			ACTION().execute<LoadMesh>( p_path );
		}
		else
		{
			{
				ACTION().execute<LoadSystem>( p_path );
			}
		}
	}

	struct _SystemIo
	{
		Util::Thread::ThreadData		 thrData;
		std::latch						 extractorCreation { 1 };
		std::optional<Extractor::System> extractor;

		inline void wait() noexcept
		{
			this->extractorCreation.wait();
			if ( this->extractor )
			{
				{
					this->extractor->wait();
				}
			}
		}

		inline void start_extraction()
		{
			assert( extractor );
			extractor.value()( thrData.stopToken, thrData.thrRef );
		}
	};

	void _SystemIoDel::operator()( _SystemIo * p_ ) noexcept { delete p_; }

	LoadSystem::LoadSystem() : _data( new _SystemIo() ) {}

	LoadSystem::LoadSystem( Util::Thread::ThreadData p_ ) : _data( new _SystemIo { std::move( p_ ) } ) {}

	void LoadSystem::execute( FilePath p_path )
	{
		_data->extractor = Extractor::System( std::move( p_path ) );
		_data->extractorCreation.count_down();

		_data->start_extraction();
	}

	void LoadSystem::execute( FilePath p_path, std::string && p_buffer )
	{
		_data->extractor = Extractor::System( std::move( p_path ), std::move( p_buffer ) );
		_data->extractorCreation.count_down();

		_data->start_extraction();
	}

	void LoadSystem::wait() noexcept { _data->wait(); }

	LoadMesh::LoadMesh( Util::Thread::StopToken p_stopToken, Util::Thread::OptionalThreadReference p_threadRef ) :
		_stopToken( std::move( p_stopToken ) ), _threadRef( std::move( p_threadRef ) )
	{
	}

	void LoadMesh::execute( FilePath p_path )
	{ Extractor::Mesh( std::move( p_path ) )( std::move( _stopToken ), _threadRef ); }

	void WriteSelection::execute( FilePath p_path )
	{
		System::SelectionWriter writer( std::move( p_path ) );
		writer();
	}

	void WriteVisible::execute( FilePath p_path )
	{
		System::VisibleWriter writer( std::move( p_path ) );
		writer();
	}

	AssociateTrajectory::AssociateTrajectory() : _data( new _SystemIo() ) {}

	AssociateTrajectory::AssociateTrajectory( Util::Thread::ThreadData p_ ) : _data( new _SystemIo { std::move( p_ ) } )
	{
	}

	void AssociateTrajectory::execute( const FilePath & p_path, const Entity & p_entity )
	{
		if ( p_entity == entt::null )
		{
			VTX_ERROR( "System entity null." );
			return;
		}

		_data->extractor = Extractor::System( p_entity, p_path );
		_data->extractorCreation.count_down();
		_data->start_extraction();
	}

	void AssociateTrajectory::execute( const std::string & p_path, const Entity & p_e )
	{ execute( FilePath( p_path ), p_e ); }

	void AssociateTrajectory::wait() noexcept { _data->wait(); }

	void RunPythonScript::execute( const FilePath & p_path )
	{
#if VTX_PYTHON_BINDING
		INTERPRETOR().runScript( p_path );
#else
		VTX_WARNING( "Python is disabled. VTX will skip script <{}>.", p_path.string() );
#endif
	}

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
	{ execute( std::nullopt, std::nullopt, Util::Image::E_FORMAT::PNG, std::nullopt, std::nullopt, std::nullopt ); }

	void Snapshot::execute(
		const std::optional<FilePath> p_path,
		const std::optional<FilePath> p_filename,
		const Util::Image::E_FORMAT	  p_format,
		const std::optional<size_t>	  p_width,
		const std::optional<size_t>	  p_height,
		const std::optional<float>	  p_backgroundOpacity
	)
	{
		const FilePath directory = p_path.value_or( SESSION().getSnapshotsDir() );
		const FilePath filename	 = p_filename.value_or( FilePath( std::to_string( Util::Chrono::getTimestamp() ) ) );

		execute(
			directory / filename,
			p_format,
			p_width.value_or( RENDERER().width() ),
			p_height.value_or( RENDERER().height() ),
			p_backgroundOpacity
		);
	}

	void Snapshot::execute(
		const FilePath				p_path,
		const Util::Image::E_FORMAT p_format,
		const size_t				p_width,
		const size_t				p_height,
		const std::optional<float>	p_backgroundOpacity
	)
	{
		try
		{
			const Util::Resolution resolution = { "Export", p_width, p_height };
			std::vector<std::byte> image	  = RENDERER().snapshot( resolution, p_backgroundOpacity );
			FilePath			   path		  = Util::Image::write( p_path, p_format, p_width, p_height, image.data() );
			_writeSnapshotMetadata( path );

			VTX_INFO( "Image saved: {}", fmt::format( fmt::runtime( std::string( LOG_LINK_FORMAT ) ), path.string() ) );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Snapshot failed: {}", p_e.what() );
		}
	}

} // namespace VTX::App::Action::IO
