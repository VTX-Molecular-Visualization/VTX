#include "menu_visualization_representation_button_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				void RepresentationPresetButton::_onButtonClicked()
				{
					Model::Representation::BaseRepresentation * representation = Model::Representation::RepresentationLibrary::get().getRepresentation( _id );
					const Model::Selection &					target		   = VTX::Selection::SelectionManager::get().getSelectionModel();

					if ( target.getItems().size() > 0 )
					{
						const VTX::Model::ID moleculeId = target.getItems().begin()->first;
						Model::Molecule &	 molecule	= MVC::MvcManager::get().getModel<Model::Molecule>( moleculeId );

						VTX_ACTION( new Action::RepresentableAddRepresentation( molecule, molecule, representation ) );
					}
					else
					{
						Object3D::Scene::MapMoleculePtrFloat & mapMolFloat = VTXApp::get().getScene().getMolecules();
						if ( mapMolFloat.size() > 0 )
						{
							Model::Molecule & molecule = *mapMolFloat.begin()->first;
							VTX_ACTION( new Action::RepresentableAddRepresentation( molecule, molecule, representation ) );
						}
					}
				};

			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
