#include "load.hpp"
#include "action/select.hpp"
#include "io/path_fake.hpp"
#include "io/reader/mmtf.hpp"
#include "io/reader/obj.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void Load::enter( void * const p_arg )
		{
			IO::Path * const path = (IO::Path *)p_arg;
			_loadFile( *path );

			VTXApp::get().goToState( ID::State::VISUALIZATION );
		}

		void Load::_loadFile( const IO::Path & p_path ) const
		{
			Model::Molecule * molecule = new Model::Molecule();
			Object3D::Scene * scene	   = &( VTXApp::get().getScene() );

			IO::Reader::BaseReader<Model::Molecule> * reader = nullptr;
			if ( p_path.getExtension() == "mmtf" ) { reader = new IO::Reader::MMTF(); }
			else if ( p_path.getExtension() == "obj" )
			{
				reader = new IO::Reader::OBJ();
			}

			const IO::PathFake * fake = dynamic_cast<const IO::PathFake *>( &p_path );

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
				if ( reader == nullptr || reader->readFile( p_path, *molecule ) == false )
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

		void Load::exit() {}

	} // namespace State
} // namespace VTX
