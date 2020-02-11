#include "state_loading.hpp"
#include "io/reader/reader_mmtf.hpp"
#include "io/reader/reader_obj.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StateLoading::enter( void * const p_arg )
		{
			// std::string * path = (std::string *)p_arg;
			// while ( *path != "\n" )
			//{
			//_loadFile( path++ );
			//}
			_loadFile( (std::string *)p_arg );

			VTXApp::get().goToState( ID::State::VISUALIZATION );
		}

		void StateLoading::_loadFile( std::string * p_path ) const
		{
			Model::Molecule * molecule = new Model::Molecule();
			Object3D::Scene *	   scene	= &( VTXApp::get().getScene() );

			// VTXApp::get().addThread( new std::thread( [ molecule, scene ] {

			const IO::Path path = IO::Path( *p_path );

			// Create factory?
			IO::BaseReader * reader = nullptr;
			if ( path.getExtension() == "mmtf" ) { reader = new IO::ReaderMMTF(); }
			else if ( path.getExtension() == "obj" )
			{
				reader = new IO::ReaderOBJ();
			}

			if ( reader != nullptr && reader->readFile( path, *molecule ) )
			{
				molecule->init();
				molecule->setSelected( true );
				molecule->print();
				molecule->getTransform().rotate( glm::radians( 90.f ), VEC3F_Z );
				scene->addMolecule( molecule );
			}

			Model::Path * p = new Model::Path();
			p->init();
			scene->addPath( p );
			p->setSelected( true );

			delete reader;
		}

		void StateLoading::exit() {}

	} // namespace State
} // namespace VTX
