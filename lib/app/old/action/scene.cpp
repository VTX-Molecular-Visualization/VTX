#include "app/old/action/scene.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/util/molecule.hpp"

namespace VTX::App::Old::Action::Scene
{
	void ResetScene::execute() { App::Old::VTXApp::get().getScene().reset(); }

	void ChangeItemIndex::execute() { App::Old::VTXApp::get().getScene().changeModelsPosition( _items, _position ); }

	void ShowAllMolecules::execute()
	{
		for ( App::Old::Application::Scene::PairMoleculePtrFloat & pairMol : App::Old::VTXApp::get().getScene().getMolecules() )
		{
			Util::App::Old::Molecule::show( *pairMol.first, true, true, true );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}
} // namespace VTX::App::Old::Action::Scene
