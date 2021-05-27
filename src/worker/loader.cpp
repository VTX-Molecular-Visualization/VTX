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
				if ( itPath->extension() == ".prm" )
				{
					IO::Reader::PRM reader = IO::Reader::PRM();
					reader.readFile( *itPath, config );
					itPath = _paths.erase( itPath );
				}
				else if ( itPath->extension() == ".psf" )
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
				emit logInfo( "Loading " + path.filename().string() );
				MODE mode = _getMode( path );

				if ( mode == MODE::UNKNOWN )
				{
					emit logError( "Format not supported" );
					_pathState.emplace( &path, false );
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
						_molecules.emplace_back( molecule );
						_pathState.emplace( &path, true );
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
						_pathState.emplace( &path, false );
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
						_meshes.emplace_back( mesh );
						_pathState.emplace( &path, true );
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( mesh );
						_pathState.emplace( &path, false );
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
						_pathState.emplace( &path, true );
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Cannot load app: " + std::string( p_e.what() ) );
						_pathState.emplace( &path, false );
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
				emit logInfo( "Loading " + pair.first.filename().string() );
				MODE mode = _getMode( pair.first );

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
						_molecules.emplace_back( molecule );
					}
					catch ( const std::exception & p_e )
					{
						emit logError( "Error loading file" );
						emit logError( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
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
			FilePath extension = p_path.extension();

			if ( extension == ".sdf" || extension == ".pdb" || extension == ".mmtf" || extension == ".cif"
				 || extension == ".arc" || extension == ".xyz" || extension == ".dcd" || extension == ".mol2" )
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
