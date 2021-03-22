#include "loader.hpp"
#include "event/event_manager.hpp"
#include "io/reader/lib_assimp.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/reader/prm.hpp"
#include "io/reader/psf.hpp"
#include "io/reader/vtx.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Worker
	{
		// TODO: some cleanup, remove ptr?
		void Loader::work()
		{
			Model::Configuration::Molecule config = Model::Configuration::Molecule();

			// Load PRM or PSF file firstly.
			for ( std::vector<Path *>::iterator it = _paths.begin(); it != _paths.end(); it++ )
			{
				if ( ( *it )->extension() == ".prm" )
				{
					IO::Reader::PRM reader = IO::Reader::PRM();
					reader.readFile( **it, config );
					_paths.erase( it-- );
				}
				else if ( ( *it )->extension() == ".psf" )
				{
					IO::Reader::PSF reader = IO::Reader::PSF();
					reader.readFile( **it, config );
					_paths.erase( it-- );
				}
			}

			// Load all files.
			Tool::Chrono chrono;
			for ( const Path * path : _paths )
			{
				chrono.start();
				VTX_INFO( "Loading " + path->filename().string() );
				MODE mode = _getMode( *path );

				if ( mode == MODE::UNKNOWN )
				{
					VTX_ERROR( "Format not supported" );
				}
				else if ( mode == MODE::MOLECULE )
				{
					// Create reader.
					IO::Reader::LibChemfiles * reader = new IO::Reader::LibChemfiles();

					// Set PRM.
					Model::Molecule * molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();
					molecule->setConfiguration( config );

					// Load.
					try
					{
						reader->readFile( *path, *molecule );
						molecule->print();
						VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, molecule ) );
						VTXApp::get().getScene().addMolecule( molecule );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Error loading file" );
						VTX_ERROR( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
					}

					delete reader;
				}
				else if ( mode == MODE::MESH )
				{
					IO::Reader::LibAssimp * reader = new IO::Reader::LibAssimp();
					Model::MeshTriangle *	mesh   = MVC::MvcManager::get().instantiateModel<Model::MeshTriangle>();

					try
					{
						reader->readFile( *path, *mesh );
						mesh->print();
						VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_CREATED, mesh ) );
						VTXApp::get().getScene().addMesh( mesh );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Error loading file" );
						VTX_ERROR( p_e.what() );
						MVC::MvcManager::get().deleteModel( mesh );
					}

					delete reader;
				}
				else if ( mode == MODE::VTX )
				{
					IO::Reader::VTX * reader = new IO::Reader::VTX();

					try
					{
						reader->readFile( *path, VTXApp::get() );
						VTX_INFO( "App loaded " );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Cannot load app: " + std::string( p_e.what() ) );
					}

					delete reader;
				}

				delete path;

				chrono.stop();
				VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

			// Load all buffers.
			for ( const std::pair<Path *, std::string *> & pair : _mapFileNameBuffer )
			{
				chrono.start();
				VTX_INFO( "Loading " + pair.first->filename().string() );
				MODE mode = _getMode( *pair.first );

				if ( mode != MODE::MOLECULE )
				{
					VTX_ERROR( "Format not supported" );
				}
				else
				{
					// Create reader.
					IO::Reader::LibChemfiles * reader	= new IO::Reader::LibChemfiles();
					Model::Molecule *		   molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();

					// Load.
					try
					{
						reader->readBuffer( *pair.second, *pair.first, *molecule );
						molecule->print();
						VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, molecule ) );
						VTXApp::get().getScene().addMolecule( molecule );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Error loading file" );
						VTX_ERROR( p_e.what() );
						MVC::MvcManager::get().deleteModel( molecule );
					}

					delete reader;
				}

				delete pair.first;
				delete pair.second;

				chrono.stop();
				VTX_INFO( "Buffer treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

			_isFinished = true;
		}

		Loader::MODE Loader::_getMode( const Path & p_path ) const
		{
			Path extension = p_path.extension();

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
