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

				if ( mode == MODE::UNKNOWN ) {}
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
						_loadMolecule( molecule, reader, path );
						molecule->init();
						molecule->print();
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
				delete path;

				chrono.stop();
				VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}
		}

		void Loader::_loadMolecule( Model::Molecule * const							p_molecule,
									IO::Reader::BaseReader<Model::Molecule> * const p_reader,
									const IO::Path * const							p_path ) const
		{
			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( p_path );
			if ( fake ) { return p_reader->readBuffer( fake->read(), *p_molecule ); }
			else
			{
				return p_reader->readFile( *p_path, *p_molecule );
			}
		}

		void Loader::_loadMesh( Model::MeshTriangle * const							p_mesh,
								IO::Reader::BaseReader<Model::MeshTriangle> * const p_reader,
								const IO::Path * const								p_path ) const
		{
			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( p_path );
			if ( fake ) { return p_reader->readBuffer( fake->read(), *p_mesh ); }
			else
			{
				return p_reader->readFile( *p_path, *p_mesh );
			}
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
