#include "loader.hpp"
#include "io/path_fake.hpp"
#include "io/reader/arc.hpp"
#include "io/reader/mmtf.hpp"
#include "io/reader/obj.hpp"
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

			IO::Reader::BaseReader<Model::Molecule> * reader = nullptr;
			if ( _path->getExtension() == "mmtf" ) { reader = new IO::Reader::MMTF(); }
			else if ( _path->getExtension() == "obj" )
			{
				reader = new IO::Reader::OBJ();
			}
			else if ( _path->getExtension() == "arc" )
			{
				reader = new IO::Reader::ARC();
			}

			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( _path );

			if ( fake )
			{
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

			molecule->init();
			molecule->print();
			scene->addMolecule( molecule );

			delete reader;
		}

	} // namespace Worker
} // namespace VTX
