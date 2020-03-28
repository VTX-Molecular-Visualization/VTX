#include "loader.hpp"
#include "io/path_fake.hpp"
#include "io/reader/arc.hpp"
#include "io/reader/mmtf.hpp"
#include "io/reader/obj.hpp"
#include "io/reader/pdb.hpp"
#include "io/reader/prm.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void Loader::work()
		{
			Model::Molecule * molecule		   = nullptr;
			Model::Molecule * moleculeTopology = nullptr;
			Model::Molecule * moleculeDynamic  = nullptr;

			for ( const IO::Path * path : _paths )
			{
				VTX_INFO( "File " + path->getFileName() );

				IO::Reader::BaseReader<Model::Molecule> * reader = _createReader( path );

				if ( reader != nullptr )
				{
					molecule = new Model::Molecule();
					if ( _loadMolecule( molecule, reader, path ) == false ) { delete molecule; }
					else
					{
						if ( _isTopology( path ) ) { moleculeTopology = molecule; }
						else if ( _isDynamic( path ) )
						{
							moleculeDynamic = molecule;
						}

						if ( moleculeTopology != nullptr && moleculeDynamic != nullptr )
						{
							VTXApp::get().getScene().removeMolecule( moleculeTopology );
							VTXApp::get().getScene().removeMolecule( moleculeDynamic );
							molecule = moleculeTopology;
							molecule->seAtomPositionFrames( moleculeDynamic->getAtomPositionFrames() );
							Generic::destroy( moleculeDynamic );
							moleculeTopology = nullptr;
							moleculeDynamic	 = nullptr;
						}

						molecule->init();
						molecule->print();
						VTXApp::get().getScene().addMolecule( molecule );
					}
					delete reader;
				}
				delete path;
			}
		}

		bool Loader::_isTopology( const IO::Path * const p_path ) const
		{
			std::string extension = p_path->getExtension();
			return ( ( extension == "pdb" ) || ( extension == "mmtf" ) );
		}

		bool Loader::_isDynamic( const IO::Path * const p_path ) const
		{
			std::string extension = p_path->getExtension();
			return ( extension == "arc" );
		}

		IO::Reader::BaseReader<Model::Molecule> * Loader::_createReader( const IO::Path * const p_path ) const
		{
			std::string extension = p_path->getExtension();
			if ( extension == "mmtf" ) { return new IO::Reader::MMTF(); }
			else if ( extension == "pdb" )
			{
				return new IO::Reader::PDB();
			}
			else if ( extension == "obj" )
			{
				return new IO::Reader::OBJ();
			}
			else if ( extension == "arc" )
			{
				return new IO::Reader::ARC();
			}
			return nullptr;
		}

		bool Loader::_loadMolecule( Model::Molecule * const							p_molecule,
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

	} // namespace Worker
} // namespace VTX
