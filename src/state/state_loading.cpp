#include "state_loading.hpp"
#include "../io/reader/reader_mmtf.hpp"
#include "../model/model_molecule.hpp"
#include "../object3d/scene.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StateLoading::enter()
		{
			Model::ModelMolecule * molecule = new Model::ModelMolecule();
			Object3D::Scene *	   scene	= &( VTXApp::get().getScene() );

			// VTXApp::get().addThread( new std::thread( [ molecule, scene ] {
			IO::ReaderMMTF mmtf = IO::ReaderMMTF();
			if ( mmtf.readFile( IO::Path( DATA_DIR + "4v6x.mmtf" ), *molecule ) )
			{
				scene->addMolecule( molecule );
				molecule->printInfos();
				VTXApp::get().goToState( State::STATE_NAME::VISUALIZATION );
			}
			//} ) );
		}

		void StateLoading::exit() {}

	} // namespace State
} // namespace VTX
