#include "state_loading.hpp"
#include "../io/reader/reader_mmtf.hpp"
#include "../io/reader/reader_obj.hpp"
#include "../model/model_molecule.hpp"
#include "../object3d/scene.hpp"
#include "../vtx_app.hpp"

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
			Model::ModelMolecule * molecule = new Model::ModelMolecule();
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
				scene->addMolecule( molecule );
				molecule->print();
			}

			delete reader;
		}

		void StateLoading::exit() {}

	} // namespace State
} // namespace VTX
