#include "loader.hpp"
#include "event/event_manager.hpp"
#include "io/reader/lib_assimp.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/reader/prm.hpp"
#include "io/reader/psf.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void Loader::receiveEvent( const Event::VTXEvent & p_event )
		{
			if ( p_event.name == Event::SCENE_CLEARED )
			{
				requestInterruption();
			}
			else if ( p_event.name == Event::MOLECULE_REMOVED )
			{
				const Event::VTXEventPtr<Model::Molecule> & castedEvent
					= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

				const bool moleculeInPotentialTarget
					= std::find(
						  _moleculeTargetsForDynamics.cbegin(), _moleculeTargetsForDynamics.cend(), castedEvent.ptr )
					  != _moleculeTargetsForDynamics.cend();

				if ( moleculeInPotentialTarget )
					requestInterruption();
			}
		}

		uint Loader::_run()
		{
			VTX_EVENT( new Event::VTXEventValue<std::string>( Event::Global::UPDATE_STATUS_BAR, "Loading..." ) );
			Util::Filesystem::fillFilepathPerMode( _paths, _filepathsPerMode );

			// Load all files.
			_loadSceneFiles();

			if ( isInterruptionRequested() )
				return 0;

			Model::Configuration::Molecule config = Model::Configuration::Molecule();
			_loadConfigurationFiles( config );

			if ( isInterruptionRequested() )
				return 0;

			_loadMoleculeFiles( config );

			if ( isInterruptionRequested() )
				return 0;

			_loadTrajectoriesFiles( config );

			if ( isInterruptionRequested() )
				return 0;

			_loadMeshFiles();

			if ( isInterruptionRequested() )
				return 0;

			// Display errors for unknown files
			for ( const IO::FilePath & path : _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::UNKNOWN ) ] )
			{
				emit logError( "Error when loading " + path.path() + " : Format not supported" );
				_pathResult[ path ].state = false;
			}

			// Load all buffers.
			_loadMoleculeBuffers();

			return 1;
		}

		void Loader::_loadSceneFiles()
		{
			for ( const IO::FilePath & path : _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::SCENE ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );

				IO::Reader::SerializedObject<VTXApp> * const reader = new IO::Reader::SerializedObject<VTXApp>();

				try
				{
					reader->readFile( path, VTXApp::get() );
					_endLoadingFileSuccess( path );
				}
				catch ( const std::exception & p_e )
				{
					_endLoadingFileFail( path, p_e.what() );
				}

				delete reader;
			}
		}
		void Loader::_loadConfigurationFiles( Model::Configuration::Molecule & p_config )
		{
			for ( const IO::FilePath & path :
				  _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::CONFIGURATION ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );
				const std::string extension = path.extension();

				try
				{
					if ( extension == "prm" )
					{
						IO::Reader::PRM reader = IO::Reader::PRM();
						reader.readFile( path, p_config );
					}
					else if ( extension == "psf" )
					{
						IO::Reader::PSF reader = IO::Reader::PSF();
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
		void Loader::_loadMoleculeFiles( const Model::Configuration::Molecule & p_config )
		{
			for ( const IO::FilePath & path : _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::MOLECULE ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );

				// Create reader.
				IO::Reader::LibChemfiles * const reader = new IO::Reader::LibChemfiles( this );

				// Set PRM.
				Model::Molecule * const molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();
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
					MVC::MvcManager::get().deleteModel( molecule );
				}

				delete reader;
			}
		}
		void Loader::_loadTrajectoriesFiles( const Model::Configuration::Molecule & p_config )
		{
			for ( const IO::FilePath & path : _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::TRAJECTORY ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );

				// Create reader.
				IO::Reader::LibChemfiles * const reader = new IO::Reader::LibChemfiles( this );

				// Load.
				try
				{
					const bool dynamicAppliedOnTarget = reader->readDynamic( path, _moleculeTargetsForDynamics );

					if ( isInterruptionRequested() )
						return;

					if ( !dynamicAppliedOnTarget ) // If the dynamic doesn't match any targets, we open it as
												   // standalone
					{
						if ( _openTrajectoryAsMolecule )
						{
							Model::Molecule * const molecule
								= MVC::MvcManager::get().instantiateModel<Model::Molecule>();
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
								MVC::MvcManager::get().deleteModel( molecule );
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( Util::Filesystem::FILE_TYPE_ENUM::MESH ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );

				IO::Reader::LibAssimp * const reader = new IO::Reader::LibAssimp();
				Model::MeshTriangle * const	  mesh	 = MVC::MvcManager::get().instantiateModel<Model::MeshTriangle>();

				try
				{
					reader->readFile( path, *mesh );
					_pathResult[ path ].mesh = mesh;

					_endLoadingFileSuccess( path );
				}
				catch ( const std::exception & p_e )
				{
					_endLoadingFileFail( path, p_e.what() );
					MVC::MvcManager::get().deleteModel( mesh );
				}

				delete reader;
			}
		}

		void Loader::_loadMoleculeBuffers()
		{
			for ( const std::pair<const IO::FilePath, std::string *> & pair : _mapFileNameBuffer )
			{
				_startLoadingFile( pair.first, SOURCE_TYPE::BUFFER );

				const Util::Filesystem::FILE_TYPE_ENUM bufferType = Util::Filesystem::getFileTypeEnum( pair.first );

				if ( bufferType == Util::Filesystem::FILE_TYPE_ENUM::MOLECULE
					 || bufferType == Util::Filesystem::FILE_TYPE_ENUM::TRAJECTORY )
				{
					// Create reader.
					IO::Reader::LibChemfiles * reader	= new IO::Reader::LibChemfiles( this );
					Model::Molecule *		   molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();

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
						MVC::MvcManager::get().deleteModel( molecule );
					}

					delete reader;
				}
				else
				{
					emit logError( "Format not supported" );
				}

				delete pair.second;
			}
		}

		void Loader::_startLoadingFile( const IO::FilePath & p_path, const SOURCE_TYPE & p_sourceType )
		{
			emit logInfo( "Loading " + p_path.filename() );
			_pathResult.emplace( p_path, Result( p_sourceType ) );
			_loadingFileChrono.start();
		}
		void Loader::_endLoadingFileSuccess( const IO::FilePath & p_path )
		{
			_loadingFileChrono.stop();
			_pathResult[ p_path ].state = true;

			switch ( _pathResult[ p_path ].sourceType )
			{
			case SOURCE_TYPE::FILE:
				emit logInfo( "File " + p_path.filename() + " treated in "
							  + std::to_string( _loadingFileChrono.elapsedTime() ) + "s" );
				break;
			case SOURCE_TYPE::BUFFER:
				emit logInfo( "Buffer " + p_path.filename() + " treated in "
							  + std::to_string( _loadingFileChrono.elapsedTime() ) + "s" );
				break;
			default: break;
			}
		}
		void Loader::_endLoadingFileFail( const IO::FilePath & p_path, const std::string & p_message )
		{
			_loadingFileChrono.stop();
			_pathResult[ p_path ].state = false;

			emit logError( "Error when loading " + p_path.filename() + " : " + p_message );
		}

	} // namespace Worker
} // namespace VTX
