#include "representation_preset_button.hpp"
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
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Molecule
{
	void RepresentationPresetButton::_setupUi( const QString & p_name )
	{
		MenuToolButtonWidget::_setupUi( p_name );
		setMaximumWidth( Style::MAIN_MENU_MAX_BUTTON_PRESET_WIDTH );
	}

	void RepresentationPresetButton::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		connect( this, &QToolButton::clicked, this, &RepresentationPresetButton::_onButtonClicked );
	}

	void RepresentationPresetButton::_onButtonClicked()
	{
		Model::Representation::Representation * representation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _id );
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		if ( selection.getMoleculesMap().size() > 0 )
		{
			VTX_ACTION( new Action::RepresentableSetRepresentation( &selection, representation ) );
		}
		else
		{
			Object3D::Scene::MapMoleculePtrFloat & mapMolFloat = VTXApp::get().getScene().getMolecules();

			for ( const Object3D::Scene::PairMoleculePtrFloat & pair : mapMolFloat )
			{
				VTX_ACTION( new Action::RepresentableSetRepresentation( *pair.first, representation ) );
			}
		}
	};

} // namespace VTX::UI::Widget::MainMenu::Molecule
