#include "loader.hpp"
#include "io/path_fake.hpp"
#include "io/reader/arc.hpp"
#include "io/reader/mmtf.hpp"
#include "io/reader/obj.hpp"
#include "io/reader/pdb.hpp"
#include "io/reader/prm.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void Loader::work()
		{
			Model::Molecule * molecule = new Model::Molecule();
			Object3D::Scene * scene	   = &( VTXApp::get().getScene() );

			molecule->setName( _path->getFileName() );

			IO::Reader::BaseReader<Model::Molecule> * reader = nullptr;

			if ( _path->getExtension() == "mmtf" ) { reader = new IO::Reader::MMTF(); }
			else if ( _path->getExtension() == "pdb" )
			{
				reader = new IO::Reader::PDB();
			}
			else if ( _path->getExtension() == "obj" )
			{
				reader = new IO::Reader::OBJ();
			}
			else if ( _path->getExtension() == "arc" )
			{
				reader = new IO::Reader::ARC();
			}
			else if ( _path->getExtension() == "prm" )
			{
			}

			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( _path );
			if ( fake )
			{
				delete reader;
				reader = new IO::Reader::MMTF();
				if ( reader == nullptr || reader->readBuffer( fake->read(), *molecule ) == false )
				{
					delete reader;
					return;
				}
			}
			else
			{
				if ( reader == nullptr || reader->readFile( *_path, *molecule ) == false )
				{
					delete reader;
					return;
				}
			}

			// TODO: delete mol if parsing failed.

			molecule->init();
			molecule->print();
			scene->addMolecule( molecule );

			delete reader;
		}

	} // namespace Worker
} // namespace VTX
