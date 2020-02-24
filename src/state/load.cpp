#include "load.hpp"
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
			_loadFile( (std::string *)p_arg );

			VTXApp::get().goToState( ID::State::VISUALIZATION );
		}

		void Load::_loadFile( std::string * p_path ) const
		{
			Model::Molecule * molecule = new Model::Molecule();
			Object3D::Scene * scene	   = &( VTXApp::get().getScene() );

			// VTXApp::get().addThread( new std::thread( [ molecule, scene ] {

			const IO::Path path = IO::Path( *p_path );

			IO::Reader::BaseReader<Model::Molecule> * reader = nullptr;
			if ( path.getExtension() == "mmtf" ) { reader = new IO::Reader::MMTF(); }
			else if ( path.getExtension() == "obj" )
			{
				reader = new IO::Reader::OBJ();
			}

			if ( reader != nullptr && reader->readFile( path, *molecule ) )
			{
				molecule->init();
				molecule->setSelected( true );
				molecule->print();
				// molecule->getTransform().rotate( glm::radians( 90.f ), VEC3F_Z );
				scene->addMolecule( molecule );
			}

			// TODO: move that in scene construtor?
			Model::Path * p = Generic::create<Model::Path>();
			scene->addPath( p );
			p->setSelected( true );

			delete reader;
		}

		void Load::exit() {}

	} // namespace State
} // namespace VTX
