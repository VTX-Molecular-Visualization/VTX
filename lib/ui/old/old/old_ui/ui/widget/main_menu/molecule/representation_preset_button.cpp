#include "ui/old_ui/ui/widget/main_menu/molecule/representation_preset_button.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/old/action/representable.hpp>
#include <app/old/application/representation/base_representable.hpp>
#include <app/old/application/representation/representation_library.hpp>
#include <app/old/application/representation/representation_manager.hpp>
#include <app/old/application/representation/representation_preset.hpp>
#include <app/old/application/scene.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/application/selection/selection_manager.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/mvc.hpp>

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
		App::Old::Application::Representation::RepresentationPreset * representation
			= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( _id );
		const App::Old::Application::Selection::SelectionModel & selection
			= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();

		if ( selection.getMoleculesMap().size() > 0 )
		{
			VTX_ACTION( new App::Old::Action::Representable::SetRepresentation( &selection, representation ) );
		}
		else
		{
			App::Old::Application::Scene::MapMoleculePtrFloat & mapMolFloat = App::Old::VTXApp::get().getScene().getMolecules();

			for ( const App::Old::Application::Scene::PairMoleculePtrFloat & pair : mapMolFloat )
			{
				VTX_ACTION( new App::Old::Action::Representable::SetRepresentation( *pair.first, representation ) );
			}
		}
	};

} // namespace VTX::UI::Widget::MainMenu::Molecule
