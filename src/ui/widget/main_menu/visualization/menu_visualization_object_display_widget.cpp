#include "menu_visualization_object_display_widget.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationObjectDisplayWidget::MenuVisualizationObjectDisplayWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
		_registerEvent( Event::Global::MOLECULE_ADDED );
		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}

	MenuVisualizationObjectDisplayWidget::~MenuVisualizationObjectDisplayWidget() {}

	void MenuVisualizationObjectDisplayWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE || p_event.name == Event::Global::MOLECULE_ADDED
			 || p_event.name == Event::Global::MOLECULE_REMOVED
			 || p_event.name == Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE )
		{
			_refreshButtons();
		}
	}

	void MenuVisualizationObjectDisplayWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Selection focus
		_showWater
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleWatersVisibilityButton" );
		_showWater->setData( "Waters", ":/sprite/hide_water_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showWater, 0 );

		_showHydrogens
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleHydrogensVisibilityButton" );
		_showHydrogens->setData( "Hydrogens", ":/sprite/hide_hydrogen_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showHydrogens, 0 );

		_showSolvent
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleSolventVisibilityButton" );
		_showSolvent->setData( "Solvent", ":/sprite/hide_solvent_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showSolvent, 1 );

		_showIon = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleIonVisibilityButton" );
		_showIon->setData( "Ions", ":/sprite/show_solvent_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showIon, 1 );

		_refreshButtons();
		validate();
	}
	void MenuVisualizationObjectDisplayWidget::_setupSlots()
	{
		connect( _showWater,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleWaterVisibilityAction );

		connect( _showHydrogens,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleHydrogenVisibilityAction );

		connect( _showSolvent,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleSolventVisibilityAction );

		connect( _showIon,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleIonVisibilityAction );
	}
	void MenuVisualizationObjectDisplayWidget::localize() { setTitle( "Object Display" ); }

	void MenuVisualizationObjectDisplayWidget::_refreshButtons()
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
		_fillContainerWithTarget( molecules );

		// No molecules, buttons disabled
		if ( molecules.size() <= 0 )
		{
			_displayWaterButton( false, true );
			_displaySolventButton( false, true );
			_displayIonButton( false, true );
			_displayHydrogenButton( false, true );
		}
		else
		{
			bool displayShowWater	 = true;
			bool displayShowSolvent	 = true;
			bool displayShowIon		 = true;
			bool displayShowHydrogen = true;

			for ( const Model::Molecule * const molecule : molecules )
			{
				displayShowWater	= displayShowWater && !molecule->showWater();
				displayShowSolvent	= displayShowSolvent && !molecule->showSolvent();
				displayShowIon		= displayShowIon && !molecule->showIon();
				displayShowHydrogen = displayShowHydrogen && !molecule->showHydrogen();
			}

			_displayWaterButton( true, displayShowWater );
			_displaySolventButton( true, displayShowSolvent );
			_displayIonButton( true, displayShowIon );
			_displayHydrogenButton( true, displayShowHydrogen );
		}
	}

	void MenuVisualizationObjectDisplayWidget::_fillContainerWithTarget(
		std::unordered_set<Model::Molecule *> & p_container ) const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		// No selection => check all molecules in scene
		if ( selection.hasMolecule() )
		{
			selection.getItemsOfType<Model::Molecule>( ID::Model::MODEL_MOLECULE, p_container );
		}
		else
		{
			for ( const std::pair<Model::Molecule *, float> & molData : VTXApp::get().getScene().getMolecules() )
			{
				p_container.emplace( molData.first );
			}
		}
	}

	void MenuVisualizationObjectDisplayWidget::_displayWaterButton( const bool p_active, const bool p_show )
	{
		_showWater->setEnabled( p_active );

		const QString iconPath = p_show ? ":/sprite/show_water_icon.png" : ":/sprite/hide_water_icon.png";
		_showWater->setIcon( QIcon( iconPath ) );
	}
	void MenuVisualizationObjectDisplayWidget::_displaySolventButton( const bool p_active, const bool p_show )
	{
		_showSolvent->setEnabled( p_active );

		const QString iconPath = p_show ? ":/sprite/show_solvent_icon.png" : ":/sprite/hide_solvent_icon.png";
		_showSolvent->setIcon( QIcon( iconPath ) );
	}
	void MenuVisualizationObjectDisplayWidget::_displayHydrogenButton( const bool p_active, const bool p_show )
	{
		_showHydrogens->setEnabled( p_active );

		const QString iconPath = p_show ? ":/sprite/show_hydrogen_icon.png" : ":/sprite/hide_hydrogen_icon.png";
		_showHydrogens->setIcon( QIcon( iconPath ) );
	}
	void MenuVisualizationObjectDisplayWidget::_displayIonButton( const bool p_active, const bool p_show )
	{
		_showIon->setEnabled( p_active );

		const QString iconPath = p_show ? ":/sprite/show_ion_icon.png" : ":/sprite/hide_ion_icon.png";
		_showIon->setIcon( QIcon( iconPath ) );
	}

	void MenuVisualizationObjectDisplayWidget::_toggleWaterVisibilityAction() const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
		_fillContainerWithTarget( molecules );

		bool showWater = true;

		for ( const Model::Molecule * const molecule : molecules )
			showWater = showWater && !molecule->showWater();

		VTX_ACTION( new Action::Molecule::ChangeShowWater( molecules, showWater ) );
	}
	void MenuVisualizationObjectDisplayWidget::_toggleSolventVisibilityAction() const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
		_fillContainerWithTarget( molecules );

		bool showSolvent = true;

		for ( const Model::Molecule * const molecule : molecules )
			showSolvent = showSolvent && !molecule->showSolvent();

		VTX_ACTION( new Action::Molecule::ChangeShowSolvent( molecules, showSolvent ) );
	}
	void MenuVisualizationObjectDisplayWidget::_toggleHydrogenVisibilityAction() const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
		_fillContainerWithTarget( molecules );

		bool showHydrogen = true;

		for ( const Model::Molecule * const molecule : molecules )
			showHydrogen = showHydrogen && !molecule->showHydrogen();

		VTX_ACTION( new Action::Molecule::ChangeShowHydrogen( molecules, showHydrogen ) );
	}
	void MenuVisualizationObjectDisplayWidget::_toggleIonVisibilityAction() const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
		_fillContainerWithTarget( molecules );

		bool showIon = true;

		for ( const Model::Molecule * const molecule : molecules )
			showIon = showIon && !molecule->showIon();

		VTX_ACTION( new Action::Molecule::ChangeShowIon( molecules, showIon ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
