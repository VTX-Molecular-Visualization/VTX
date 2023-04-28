#include "ui/old_ui/ui/widget/main_menu/molecule/representation_preset_button.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/action/representable.hpp>
#include <app/application/representation/base_representable.hpp>
#include <app/application/representation/representation_library.hpp>
#include <app/application/representation/representation_manager.hpp>
#include <app/application/representation/representation_preset.hpp>
#include <app/application/scene.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/mvc.hpp>

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
		App::Application::Representation::RepresentationPreset * representation
			= App::Application::Representation::RepresentationLibrary::get().getRepresentation( _id );
		const App::Application::Selection::SelectionModel & selection
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

		if ( selection.getMoleculesMap().size() > 0 )
		{
			VTX_ACTION( new App::Action::Representable::SetRepresentation( &selection, representation ) );
		}
		else
		{
			App::Application::Scene::MapMoleculePtrFloat & mapMolFloat = VTXApp::get().getScene().getMolecules();

			for ( const App::Application::Scene::PairMoleculePtrFloat & pair : mapMolFloat )
			{
				VTX_ACTION( new App::Action::Representable::SetRepresentation( *pair.first, representation ) );
			}
		}
	};

} // namespace VTX::UI::Widget::MainMenu::Molecule
