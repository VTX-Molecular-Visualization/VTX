#include "app/internal/worker/loader.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/io/molecule_configuration.hpp"
#include "app/component/object3d/mesh_triangle.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/event.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/internal/io/reader/lib_assimp.hpp"
#include "app/internal/io/reader/lib_chemfiles.hpp"
#include "app/internal/io/reader/prm.hpp"
#include "app/internal/io/reader/psf.hpp"
#include "app/mvc.hpp"
#include "app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App::Internal::Worker
{
	uint Loader::_run()
	{
		App::Internal::IO::Filesystem::fillFilepathPerMode( _paths, _filepathsPerMode );

		// Load all files.
		_loadSceneFiles();
		App::Component::IO::MoleculeConfiguration config = App::Component::IO::MoleculeConfiguration();
		_loadConfigurationFiles( config );
		_loadMoleculeFiles( config );
		_loadTrajectoriesFiles( config );
		_loadMeshFiles();

		// Display errors for unknown files
		for ( const FilePath & path :
			  _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::UNKNOWN ) ] )
		{
			emitLogError( "Error when loading " + path.string() + " : Format not supported" );
			_pathResult[ path ].state = false;
		}

		// Load all buffers.
		_loadMoleculeBuffers();

		return 1;
	}

	void Loader::_loadSceneFiles()
	{
		for ( const FilePath & path : _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::SCENE ) ] )
		{
			_startLoadingFile( path, SOURCE_TYPE::FILE );

			App::Core::IO::Reader::SerializedObject<VTXApp> * const reader
				= new App::Core::IO::Reader::SerializedObject<VTXApp>();

			try
			{
				reader->readFile( path, App::VTXApp::get() );
				_endLoadingFileSuccess( path );
			}
			catch ( const std::exception & p_e )
			{
				_endLoadingFileFail( path, p_e.what() );
			}

			delete reader;
		}
	}
	void Loader::_loadConfigurationFiles( App::Component::IO::MoleculeConfiguration & p_config )
	{
		for ( const FilePath & path :
			  _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::CONFIGURATION ) ] )
		{
			_startLoadingFile( path, SOURCE_TYPE::FILE );
			const std::string extension = path.extension().string();

			try
			{
				if ( extension == "prm" )
				{
					App::Internal::IO::Reader::PRM reader = App::Internal::IO::Reader::PRM();
					reader.readFile( path, p_config );
				}
				else if ( extension == "psf" )
				{
					App::Internal::IO::Reader::PSF reader = App::Internal::IO::Reader::PSF();
					reader.readFile( path, p_config );
				}

				_endLoadingFileSuccess( path );
			}
			catch ( const std::exception & p_e )
			{
				_endLoadingFileFail( path, p_e.what() );
			}
		}
	}
	void Loader::_loadMoleculeFiles( const App::Component::IO::MoleculeConfiguration & p_config )
	{
		for ( const FilePath & path :
			  _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::MOLECULE ) ] )
		{
			_startLoadingFile( path, SOURCE_TYPE::FILE );

			// Create reader.
			App::Internal::IO::Reader::LibChemfiles * const reader
				= new App::Internal::IO::Reader::LibChemfiles( this );

			// Set PRM.
			App::Component::Chemistry::Molecule * const molecule
				= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::Molecule>();
			molecule->setConfiguration( p_config );

			// Load.
			try
			{
				reader->readFile( path, *molecule );
				_pathResult[ path ].molecule = molecule;
				_moleculeTargetsForDynamics.emplace_back( molecule );
				_endLoadingFileSuccess( path );
			}
			catch ( const std::exception & p_e )
			{
				_endLoadingFileFail( path, p_e.what() );
				VTX::MVC_MANAGER().deleteModel( molecule );
			}

			delete reader;
		}
	}
	void Loader::_loadTrajectoriesFiles( const App::Component::IO::MoleculeConfiguration & p_config )
	{
		for ( const FilePath & path :
			  _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::TRAJECTORY ) ] )
		{
			_startLoadingFile( path, SOURCE_TYPE::FILE );

			// Create reader.
			App::Internal::IO::Reader::LibChemfiles * const reader
				= new App::Internal::IO::Reader::LibChemfiles( this );

			// Load.
			try
			{
				const bool dynamicAppliedOnTarget = reader->readDynamic( path, _moleculeTargetsForDynamics );

				if ( !dynamicAppliedOnTarget ) // If the dynamic doesn't match any targets, we open it as standalone
				{
					if ( _openTrajectoryAsMolecule )
					{
						App::Component::Chemistry::Molecule * const molecule
							= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::Molecule>();
						molecule->setConfiguration( p_config );

						// Load.
						try
						{
							reader->readFile( path, *molecule );
							_pathResult[ path ].molecule = molecule;
							_endLoadingFileSuccess( path );
						}
						catch ( const std::exception & p_e )
						{
							_endLoadingFileFail( path, p_e.what() );
							VTX::MVC_MANAGER().deleteModel( molecule );
						}
					}
					else
					{
						_endLoadingFileFail( path, "Trajectory doesn't match any target." );
					}
				}
				else
				{
					_endLoadingFileSuccess( path );
				}
			}
			catch ( const std::exception & p_e )
			{
				_endLoadingFileFail( path, p_e.what() );
			}

			delete reader;
		}
	}
	void Loader::_loadMeshFiles()
	{
		for ( const FilePath & path : _filepathsPerMode[ int( App::Internal::IO::Filesystem::FILE_TYPE_ENUM::MESH ) ] )
		{
			_startLoadingFile( path, SOURCE_TYPE::FILE );

			App::Internal::IO::Reader::LibAssimp * const   reader = new App::Internal::IO::Reader::LibAssimp();
			App::Component::Object3D::MeshTriangle * const mesh
				= VTX::MVC_MANAGER().instantiateModel<App::Component::Object3D::MeshTriangle>();

			try
			{
				reader->readFile( path, *mesh );
				_pathResult[ path ].mesh = mesh;

				_endLoadingFileSuccess( path );
			}
			catch ( const std::exception & p_e )
			{
				_endLoadingFileFail( path, p_e.what() );
				VTX::MVC_MANAGER().deleteModel( mesh );
			}

			delete reader;
		}
	}

	void Loader::_loadMoleculeBuffers()
	{
		for ( const std::pair<const FilePath, std::string *> & pair : _mapFileNameBuffer )
		{
			_startLoadingFile( pair.first, SOURCE_TYPE::BUFFER );

			const App::Internal::IO::Filesystem::FILE_TYPE_ENUM bufferType
				= App::Internal::IO::Filesystem::getFileTypeFromFilePath( pair.first );

			if ( bufferType == App::Internal::IO::Filesystem::FILE_TYPE_ENUM::MOLECULE
				 || bufferType == App::Internal::IO::Filesystem::FILE_TYPE_ENUM::TRAJECTORY )
			{
				// Create reader.
				App::Internal::IO::Reader::LibChemfiles * reader = new App::Internal::IO::Reader::LibChemfiles( this );
				App::Component::Chemistry::Molecule *	  molecule
					= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::Molecule>();

				// Load.
				try
				{
					reader->readBuffer( *pair.second, pair.first, *molecule );
					_pathResult[ pair.first ].molecule = molecule;
					_endLoadingFileSuccess( pair.first );
				}
				catch ( const std::exception & p_e )
				{
					_endLoadingFileFail( pair.first, p_e.what() );
					VTX::MVC_MANAGER().deleteModel( molecule );
				}

				delete reader;
			}
			else
			{
				emitLogError( "Format not supported" );
			}

			delete pair.second;
		}
	}

	void Loader::_startLoadingFile( const FilePath & p_path, const SOURCE_TYPE & p_sourceType )
	{
		emitLogInfo( "Loading " + p_path.filename().string() );
		_pathResult.emplace( p_path, Result( p_sourceType ) );
		_loadingFileChrono.start();
	}
	void Loader::_endLoadingFileSuccess( const FilePath & p_path )
	{
		_loadingFileChrono.stop();
		_pathResult[ p_path ].state = true;

		switch ( _pathResult[ p_path ].sourceType )
		{
		case SOURCE_TYPE::FILE:
			emitLogInfo( "File " + p_path.filename().string() + " treated in "
						 + std::to_string( _loadingFileChrono.elapsedTime() ) + "s" );
			break;
		case SOURCE_TYPE::BUFFER:
			emitLogInfo( "Buffer " + p_path.filename().string() + " treated in "
						 + std::to_string( _loadingFileChrono.elapsedTime() ) + "s" );
			break;
		default: break;
		}
	}
	void Loader::_endLoadingFileFail( const FilePath & p_path, const std::string & p_message )
	{
		_loadingFileChrono.stop();
		_pathResult[ p_path ].state = false;

		emitLogError( "Error when loading " + p_path.filename().string() + " : " + p_message );
	}

} // namespace VTX::App::Internal::Worker
