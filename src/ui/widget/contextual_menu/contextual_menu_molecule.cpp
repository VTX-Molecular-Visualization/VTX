#include "contextual_menu_molecule.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "action/visible.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuMolecule::ContextualMenuMolecule( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuMolecule ::~ContextualMenuMolecule() {}

	void ContextualMenuMolecule::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_representationMenu
			= WidgetFactory::get().instantiateWidget<CustomWidget::SetRepresentationMenu>( this, "RepresentationMenu" );
	}
	void ContextualMenuMolecule::_setupSlots()
	{
		addAction( "Rename", this, &ContextualMenuMolecule::_renameAction );
		addSeparator();
		addMenu( _representationMenu );
		addSeparator();
		_toggleWaterAction = addAction( "Toggle Waters", this, &ContextualMenuMolecule::_toggleWatersVisibilityAction );
		_toggleSolventAction
			= addAction( "Toggle Solvent", this, &ContextualMenuMolecule::_toggleSolventVisibilityAction );
		_toggleHydrogenAction
			= addAction( "Toggle Hydrogen", this, &ContextualMenuMolecule::_toggleHydrogensVisibilityAction );
		addSeparator();
		addAction( "Orient", this, &ContextualMenuMolecule::_orientAction );
		addAction( "Show", this, &ContextualMenuMolecule::_showAction );
		addAction( "Hide", this, &ContextualMenuMolecule::_hideAction );
		addAction( "Copy", this, &ContextualMenuMolecule::_copyAction );
		addAction( "Delete", this, &ContextualMenuMolecule::_deleteAction, QKeySequence::Delete );

		connect( _representationMenu,
				 &CustomWidget::SetRepresentationMenu ::onRepresentationChange,
				 this,
				 &ContextualMenuMolecule::_applyRepresentationAction );
	}

	void ContextualMenuMolecule::localize() {}

	void ContextualMenuMolecule::setTarget( Model::Molecule * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getPdbIdCode() ) );

		const int representationIndex = Model::Representation::RepresentationLibrary::get().getRepresentationIndex(
			p_target->getRepresentation()->getLinkedRepresentation() );

		_toggleWaterAction->setText( p_target->showWater() ? "Hide Waters" : "Show Waters" );
		_toggleSolventAction->setText( p_target->showSolvent() ? "Hide Solvent" : "Show Solvent" );
		_toggleHydrogenAction->setText( p_target->showHydrogen() ? "Hide Hydrogens" : "Show Hydrogens" );

		_representationMenu->tickCurrentRepresentation( representationIndex );
	}

	void ContextualMenuMolecule::_renameAction()
	{
		View::UI::Widget::MoleculeSceneView * const molSceneView
			= MVC::MvcManager::get().getView<View::UI::Widget::MoleculeSceneView>( _target,
																				   ID::View::UI_MOLECULE_STRUCTURE );

		molSceneView->openRenameEditor();
	}
	void ContextualMenuMolecule::_toggleWatersVisibilityAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeShowWater( *_target, !_target->showWater() ) );
	}
	void ContextualMenuMolecule::_toggleSolventVisibilityAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeShowSolvent( *_target, !_target->showSolvent() ) );
	}
	void ContextualMenuMolecule::_toggleHydrogensVisibilityAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeShowHydrogen( *_target, !_target->showHydrogen() ) );
	}
	void ContextualMenuMolecule::_orientAction() { VTX_ACTION( new Action::Molecule::Orient( *_target ) ); }
	void ContextualMenuMolecule::_showAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeVisibility( *_target,
															Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuMolecule::_hideAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeVisibility(
			*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuMolecule::_copyAction() { VTX_ACTION( new Action::Molecule::Copy( *_target ) ); }
	void ContextualMenuMolecule::_deleteAction() { VTX_ACTION( new Action::Molecule::Delete( *_target ) ); }

	void ContextualMenuMolecule::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new Action::Molecule::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
