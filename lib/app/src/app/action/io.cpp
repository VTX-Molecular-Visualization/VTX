#include "app/action/io.hpp"
#include "app/constants.hpp"
#include "app/extractor/mesh.hpp"
#include "app/extractor/system.hpp"
#include "app/helper/io.hpp"
#include <util/action_manager.hpp>
#if VTX_PYTHON_BINDING
#include "app/python_binding/interpretor.hpp"
#endif
#include "app/arguments.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include "app/system/writer.hpp"
#include <fmt/format.h>
#include <io/file_format.hpp>
#include <io/mesh_reader.hpp>
#include <memory>
#include <renderer/renderer.hpp>
#include <string>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/resolution.hpp>

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

	void Open::execute( FilePath p_path )
	{
		if ( VTX::IO::isScriptFileFormat( p_path ) )
		{
			if ( ARGS().noPython )
			{
				VTX_WARNING( "Python is disabled. VTX will skip script <{}>.", p_path.string() );
				return;
			}

			ACTION().execute<RunPythonScript>( std::move( p_path ) );
		}
		else if ( VTX::IO::MeshReader::isMeshFileFormat( p_path ) )
		{
			ACTION().execute<LoadMesh>( std::move( p_path ) );
		}
		else
		{
			{
				ACTION().execute<LoadSystem>( std::move( p_path ) );
			}
		}
	}

	LoadSystem::LoadSystem( Util::Thread::ThreadData p_threadData ) : _threadData( std::move( p_threadData ) ) {}

	void LoadSystem::execute( FilePath p_path )
	{
		Extractor::System extractor( std::move( p_path ) );
		extractor( _threadData.stopToken, _threadData.thrRef );
	}

	void LoadSystem::execute( FilePath p_path, std::string && p_buffer )
	{
		Extractor::System extractor( std::move( p_path ), std::move( p_buffer ) );
		extractor( _threadData.stopToken, _threadData.thrRef );
	}

	LoadMesh::LoadMesh( Util::Thread::ThreadData p_threadData ) : _threadData( std::move( p_threadData ) ) {}

	void LoadMesh::execute( FilePath p_path )
	{ Extractor::Mesh( std::move( p_path ) )( _threadData.stopToken, _threadData.thrRef ); }

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

	AssociateTrajectory::AssociateTrajectory( Util::Thread::ThreadData p_threadData ) :
		_threadData( std::move( p_threadData ) )
	{
	}

	void AssociateTrajectory::execute( FilePath p_path, const Entity & p_entity )
	{
		if ( p_entity == InvalidEntity )
		{
			VTX_ERROR( "System entity null." );
			return;
		}

		Extractor::System extractor( p_entity, std::move( p_path ) );
		extractor( _threadData.stopToken, _threadData.thrRef );
	}

	void AssociateTrajectory::execute( const std::string & p_path, const Entity & p_e )
	{
		if ( p_e == InvalidEntity )
		{
			VTX_ERROR( "System entity null." );
			return;
		}
		Extractor::System extractor( p_e, p_path );
		extractor( _threadData.stopToken, _threadData.thrRef );
	}

	void RunPythonScript::execute( const FilePath & p_path )
	{
#if VTX_PYTHON_BINDING
		INTERPRETOR().runScript( p_path );
#else
		VTX_WARNING( "Python is disabled. VTX will skip script <{}>.", p_path.string() );
#endif
	}

	void DownloadSystem::execute( const std::string & p_systemId )
	{ execute( Helper::IO::getSystemDownloadUrl( p_systemId ), Helper::IO::getSystemDownloadFilename( p_systemId ) ); }

	DownloadSystem::DownloadSystem( Util::Thread::ThreadData p_threadData ) : _threadData( std::move( p_threadData ) )
	{
	}

	void DownloadSystem::execute( const std::string & p_url, FilePath p_path )
	{
		if ( _threadData.thrRef )
		{
			_threadData.thrRef->get().setProgressText( "Downloading " + p_path.string() );
		}
		const Util::Network::ProgressCallback progress = [ this ]( const size_t p_downloaded, const size_t p_total )
		{
			if ( p_total > 0 && _threadData.thrRef )
			{
				_threadData.thrRef->get().setProgress(
					static_cast<float>( p_downloaded ) / static_cast<float>( p_total )
				);
			}
			return not _threadData.stopToken.stop_requested();
		};

		std::optional<std::string> data = Helper::IO::fetch( p_url, progress );
		if ( not data )
		{
			return;
		}
		Helper::IO::writeCache( p_path.string(), *data );

		LoadSystem( _threadData ).execute( std::move( p_path ), std::move( *data ) );
	}

	void Snapshot::execute()
	{ execute( std::nullopt, std::nullopt, Util::Image::E_FORMAT::PNG, std::nullopt, std::nullopt, std::nullopt ); }

	void Snapshot::execute(
		const std::optional<FilePath> & p_path,
		const std::optional<FilePath> & p_filename,
		const Util::Image::E_FORMAT		p_format,
		const std::optional<size_t>		p_width,
		const std::optional<size_t>		p_height,
		const std::optional<float>		p_backgroundOpacity
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
		const FilePath &			p_path,
		const Util::Image::E_FORMAT p_format,
		const size_t				p_width,
		const size_t				p_height,
		const std::optional<float>	p_backgroundOpacity
	)
	{
		const Util::Resolution resolution = { "Export", p_width, p_height };
		std::vector<std::byte> image	  = RENDERER().snapshot( resolution, p_backgroundOpacity );
		const FilePath		   path		  = Util::Image::write( p_path, p_format, p_width, p_height, image.data() );
		_writeSnapshotMetadata( path );

		VTX_INFO( "Image saved: {}", fmt::format( fmt::runtime( std::string( LOG_LINK_FORMAT ) ), path.string() ) );
	}

} // namespace VTX::App::Action::IO
