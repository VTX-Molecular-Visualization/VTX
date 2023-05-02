#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/util/molecule.hpp"

namespace VTX::App::Action::Scene
{
	void ResetScene::execute() { VTXApp::get().getScene().reset(); }

	void ChangeItemIndex::execute() { VTXApp::get().getScene().changeModelsPosition( _items, _position ); }

	void ShowAllMolecules::execute()
	{
		for ( App::Application::Scene::PairMoleculePtrFloat & pairMol : VTXApp::get().getScene().getMolecules() )
		{
			Util::App::Molecule::show( *pairMol.first, true, true, true );
		}

		VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}
} // namespace VTX::App::Action::Scene
