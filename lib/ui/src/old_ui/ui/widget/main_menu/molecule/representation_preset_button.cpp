#include "ui/old_ui/ui/widget/main_menu/molecule/representation_preset_button.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/action/representable.hpp>
#include <app/core/action/action_manager.hpp>
#include <app/old_app/generic/base_representable.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/representation/representation.hpp>
#include <app/old_app/model/representation/representation_library.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/representation/representation_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>

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
			VTX_ACTION( new Action::Representable::SetRepresentation( &selection, representation ) );
		}
		else
		{
			Object3D::Scene::MapMoleculePtrFloat & mapMolFloat = VTXApp::get().getScene().getMolecules();

			for ( const Object3D::Scene::PairMoleculePtrFloat & pair : mapMolFloat )
			{
				VTX_ACTION( new Action::Representable::SetRepresentation( *pair.first, representation ) );
			}
		}
	};

} // namespace VTX::UI::Widget::MainMenu::Molecule
