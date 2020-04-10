#include "loader.hpp"
#include "io/path_fake.hpp"
#include "io/reader/lib_assimp.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/reader/lib_mmtf.hpp"
#include "io/reader/prm.hpp"
#include "tool/chrono.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void Loader::work()
		{
			// Load PRM file firstly.
			IO::Reader::PRMFile prm;
			for ( const IO::Path * path : _paths )
			{
				if ( path->getExtension() == "prm" )
				{
					IO::Reader::PRM reader = IO::Reader::PRM();
					reader.readFile( *path, prm );
				}
			}

			// Load all files.
			Tool::Chrono chrono;
			for ( const IO::Path * path : _paths )
			{
				chrono.start();
				VTX_INFO( "Loading " + path->getFileName() );
				MODE mode = _getMode( *path );

				if ( mode == MODE::UNKNOWN ) { VTX_ERROR("Format not supported"); }
				else if ( mode == MODE::MOLECULE )
				{
					// Create reader.
					IO::Reader::BaseReader<Model::Molecule> * reader;
					if ( path->getExtension() == "mmtf" ) { reader = new IO::Reader::LibMMTF(); }
					else
					{
						reader = new IO::Reader::LibChemfiles();
					}

					// Set PRM.
					Model::Molecule * molecule = new Model::Molecule();
					molecule->setPRM( prm );

					// Load.
					try
					{
						_load( molecule, reader, path );
						VTXApp::get().getScene().addMolecule( molecule );
					}
					catch ( const std::exception & p_e )
					{
						delete molecule;
						VTX_ERROR( "Error loading file" );
						VTX_ERROR( p_e.what() );
					}

					delete reader;
				}
				else if ( mode == MODE::MESH )
				{
					IO::Reader::BaseReader<Model::MeshTriangle> * reader = new IO::Reader::LibAssimp();
					Model::MeshTriangle *						  mesh = new Model::MeshTriangle();

					try
					{
						_load( mesh, reader, path );
						VTXApp::get().getScene().addMesh( mesh );
					}
					catch ( const std::exception & p_e )
					{
						delete mesh;
						VTX_ERROR( "Error loading file" );
						VTX_ERROR( p_e.what() );
					}

					delete reader;
				}

				delete path;

				chrono.stop();
				VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}
		}

		template<typename T>
		void Loader::_load( T * const						  p_data,
							IO::Reader::BaseReader<T> * const p_reader,
							const IO::Path * const			  p_path ) const
		{
			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( p_path );
			if ( fake ) { p_reader->readBuffer( fake->read(), *p_data ); }
			else
			{
				p_reader->readFile( *p_path, *p_data );
			}
			p_data->init();
			p_data->print();
		}

		Loader::MODE Loader::_getMode( const IO::Path & p_path ) const
		{
			std::string extension = p_path.getExtension();
			if ( extension == "mmtf" ) { return MODE::MOLECULE; }
			else if ( extension == "pdb" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == "arc" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == "xyz" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == "dcd" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == "obj" )
			{
				return MODE::MESH;
			}
			else
			{
				return MODE::UNKNOWN;
			}
		}

	} // namespace Worker
} // namespace VTX
