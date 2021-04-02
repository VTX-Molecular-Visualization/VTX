#include "menu_visualization_representation_button_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu
{
	void RepresentationPresetButton::_onButtonClicked()
	{
		Model::Representation::Representation * representation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _id );
		const Model::Selection & target = VTX::Selection::SelectionManager::get().getSelectionModel();

		if ( target.getItems().size() > 0 )
		{
			VTX_ACTION( new Action::RepresentableSetRepresentation( &target, representation ) );
		}
		else
		{
			Object3D::Scene::MapMoleculePtrFloat & mapMolFloat = VTXApp::get().getScene().getMolecules();
			if ( mapMolFloat.size() > 0 )
			{
				Model::Molecule & molecule = *mapMolFloat.begin()->first;
				VTX_ACTION( new Action::RepresentableSetRepresentation( molecule, representation ) );
			}
		}
	};

} // namespace VTX::UI::Widget::MainMenu
