#include "loading.hpp"
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
		void Loading::enter( void * const p_arg )
		{
			// std::string * path = (std::string *)p_arg;
			// while ( *path != "\n" )
			//{
			//_loadFile( path++ );
			//}
			_loadFile( (std::string *)p_arg );

			VTXApp::get().goToState( ID::State::VISUALIZATION );
		}

		void Loading::_loadFile( std::string * p_path ) const
		{
			Model::Molecule * molecule = new Model::Molecule();
			Object3D::Scene * scene	   = &( VTXApp::get().getScene() );

			// VTXApp::get().addThread( new std::thread( [ molecule, scene ] {

			const IO::Path path = IO::Path( *p_path );

			// Create factory?
			IO::Reader::BaseReader * reader = nullptr;
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

			Model::Path * p = Generic::Factory<Model::Path, Model::Viewpoint>::create();
			scene->addPath( p );
			p->setSelected( true );

			delete reader;
		}

		void Loading::exit() {}

	} // namespace State
} // namespace VTX
