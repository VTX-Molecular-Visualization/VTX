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
			Model::Configuration::Molecule config = Model::Configuration::Molecule();

			// Load PRM or PSF file firstly.
			std::vector<FilePath>::iterator itPath = _paths.begin();
			while ( itPath != _paths.end() )
			{
				const std::string extension = Util::Filesystem::getFileExtension( *itPath );
				if ( extension == "prm" )
				{
					IO::Reader::PRM reader = IO::Reader::PRM();
					reader.readFile( *itPath, config );
					itPath = _paths.erase( itPath );
				}
				else if ( extension == "psf" )
				{
					IO::Reader::PSF reader = IO::Reader::PSF();
					reader.readFile( *itPath, config );
					itPath = _paths.erase( itPath );
				}
				else
				{
					++itPath;
				}
			}

			// Load all files.
			Tool::Chrono chrono;
			for ( FilePath & path : _paths )
			{
				chrono.start();
				emit logInfo( "Loading " + Util::Filesystem::getFileName( path ) );
				MODE mode = _getMode( path );

				_pathResult.emplace( path, Result( SOURCE_TYPE::FILE ) );

				if ( mode == MODE::UNKNOWN )
				{
					emit logError( "Format not supported" );
					_pathResult[ path ].state = false;
				}
				else if ( mode == MODE::MOLECULE )
				{
					// Create reader.
					IO::Reader::LibChemfiles * const reader = new IO::Reader::LibChemfiles( this );

					// Set PRM.
					Model::Molecule * const molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();
					molecule->setConfiguration( config );

					// Load.
					try
					{
						reader->readFile( path, *molecule );
						_pathResult[ path ].molecule = molecule;
						_pathResult[ path ].state	 = true;
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
						_pathResult[ path ].state = false;
					}

					delete reader;
				}
				else if ( mode == MODE::MESH )
				{
					IO::Reader::LibAssimp * const reader = new IO::Reader::LibAssimp();
					Model::MeshTriangle * const	  mesh = MVC::MvcManager::get().instantiateModel<Model::MeshTriangle>();

					try
					{
						reader->readFile( path, *mesh );
						_pathResult[ path ].mesh  = mesh;
						_pathResult[ path ].state = true;
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( mesh );
						_pathResult[ path ].state = false;
					}

					delete reader;
				}
				else if ( mode == MODE::VTX )
				{
					IO::Reader::SerializedObject<VTXApp> * const reader = new IO::Reader::SerializedObject<VTXApp>();

					try
					{
						reader->readFile( path, VTXApp::get() );
						emit logInfo( "App loaded " );
						_pathResult[ path ].state = true;
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Cannot load app: " + std::string( p_e.what() ) );
						_pathResult[ path ].state = false;
					}

					delete reader;
				}

				// Path deleted in callback => save path in recent path when loading works
				// delete path;

				chrono.stop();
				emit logInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

			// Load all buffers.
			for ( const std::pair<FilePath, std::string *> & pair : _mapFileNameBuffer )
			{
				chrono.start();
				emit logInfo( "Loading " + Util::Filesystem::getFileName( pair.first ) );
				MODE mode = _getMode( pair.first );

				_pathResult.emplace( pair.first, Result( SOURCE_TYPE::BUFFER ) );

				if ( mode != MODE::MOLECULE )
				{
					emit logError( "Format not supported" );
				}
				else
				{
					// Create reader.
					IO::Reader::LibChemfiles * reader	= new IO::Reader::LibChemfiles( this );
					Model::Molecule *		   molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();

					// Load.
					try
					{
						reader->readBuffer( *pair.second, pair.first, *molecule );
						_pathResult[ pair.first ].molecule = molecule;
						_pathResult[ pair.first ].state	   = true;
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
						_pathResult[ pair.first ].state = false;
					}

					delete reader;
				}

				delete pair.second;

				chrono.stop();
				emit logInfo( "Buffer treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

			return 1;
		}

		Loader::MODE Loader::_getMode( const FilePath & p_path ) const
		{
			FilePath extension = Util::Filesystem::getFileExtension( p_path );

			if ( extension == "nc" || extension == "cif" || extension == "cml" || extension == "cssr"
				 || extension == "dcd" || extension == "gro" || extension == "lammpstrj" || extension == "mmcif"
				 || extension == "mmtf" || extension == "mol2" || extension == "molden" || extension == "pdb"
				 || extension == "sdf" || extension == "smi" || extension == "arc" || extension == "trr"
				 || extension == "mmtf" || extension == "xtc" || extension == "tng" || extension == "trj"
				 || extension == "xyz" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == ".obj" )
			{
				return MODE::MESH;
			}
			else if ( extension == ".vtx" )
			{
				return MODE::VTX;
			}
			else
			{
				return MODE::UNKNOWN;
			}
		}

	} // namespace Worker
} // namespace VTX
