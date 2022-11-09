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
		uint Loader::_run()
		{
			_fillFilepathPerMode();

			// Load all files.
			_loadSceneFiles();
			Model::Configuration::Molecule config = Model::Configuration::Molecule();
			_loadConfigurationFiles( config );
			_loadMoleculeFiles( config );
			_loadTrajectoriesFiles( config );
			_loadMeshFiles();

			// Display errors for unknown files
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::UNKNOWN ) ] )
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::SCENE ) ] )
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::CONFIGURATION ) ] )
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::MOLECULE ) ] )
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::TRAJECTORY ) ] )
			{
				_startLoadingFile( path, SOURCE_TYPE::FILE );

				// Create reader.
				IO::Reader::LibChemfiles * const reader = new IO::Reader::LibChemfiles( this );

				// Load.
				try
				{
					const bool dynamicAppliedOnTarget = reader->readDynamic( path, _moleculeTargetsForDynamics );

					if ( !dynamicAppliedOnTarget ) // If the dynamic doesn't match any targets, we open it as standalone
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
			for ( const IO::FilePath & path : _filepathsPerMode[ int( MODE::SCENE ) ] )
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

				const MODE bufferType = _getMode( pair.first );

				if ( bufferType == MODE::MOLECULE || bufferType == MODE::TRAJECTORY )
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

		void Loader::_fillFilepathPerMode()
		{
			_filepathsPerMode.resize( int( MODE::COUNT ) );

			for ( const IO::FilePath & path : _paths )
			{
				MODE filetype = _getMode( path );
				_filepathsPerMode[ int( filetype ) ].emplace_back( path );
			}
		}

		Loader::MODE Loader::_getMode( const IO::FilePath & p_path ) const
		{
			std::string extension = p_path.extension();

			if ( extension == ".vtx" )
			{
				return MODE::SCENE;
			}
			else if ( extension == "prm" || extension == "psf" )
			{
				return MODE::CONFIGURATION;
			}
			else if ( extension == "cif" || extension == "cml" || extension == "cssr" || extension == "gro"
					  || extension == "mmcif" || extension == "mmtf" || extension == "mol2" || extension == "molden"
					  || extension == "pdb" || extension == "sdf" || extension == "smi" || extension == "mmtf"
					  || extension == "xyz" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == ".obj" )
			{
				return MODE::MESH;
			}
			else if ( extension == "nc" || extension == "dcd" || extension == "lammpstrj" || extension == "arc"
					  || extension == "trr" || extension == "xtc" || extension == "tng" || extension == "trj" )
			{
				return MODE::TRAJECTORY;
			}
			else
			{
				return MODE::UNKNOWN;
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
