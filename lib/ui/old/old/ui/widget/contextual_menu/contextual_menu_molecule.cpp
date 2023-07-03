#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_molecule.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/view/ui/widget/molecule_scene_view.hpp"
#include "ui/qt/action/molecule.hpp"
#include <app/old/action/molecule.hpp>
#include <app/old/action/visible.hpp>
#include <app/old/application/representation/representation_library.hpp>

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
		_toggleWaterAction = addAction( "Toggle waters", this, &ContextualMenuMolecule::_toggleWatersVisibilityAction );
		_toggleHydrogenAction
			= addAction( "Toggle hydrogens", this, &ContextualMenuMolecule::_toggleHydrogensVisibilityAction );
		_toggleSolventAction
			= addAction( "Toggle solvent", this, &ContextualMenuMolecule::_toggleSolventVisibilityAction );
		_toggleIonAction = addAction( "Toggle ions", this, &ContextualMenuMolecule::_toggleIonVisibilityAction );

		addSeparator();
		_toggleTrajectoryPlayingAction
			= addAction( "Toggle Play", this, &ContextualMenuMolecule::_toggleTrajectoryPlayingActions );
		addAction( "Load Trajectory", this, &ContextualMenuMolecule::_loadTrajectoryAction );

		addSeparator();
		addAction( "Orient", this, &ContextualMenuMolecule::_orientAction );
		addAction( "Show", this, &ContextualMenuMolecule::_showAction );
		addAction( "Hide", this, &ContextualMenuMolecule::_hideAction );
		addAction( "Solo", this, &ContextualMenuMolecule::_soloAction );
		addAction( "Duplicate", this, &ContextualMenuMolecule::_copyAction );
		addAction( "Delete", this, &ContextualMenuMolecule::_deleteAction );

		addSeparator();
		addAction( "Export", this, &ContextualMenuMolecule::_exportAction );

		connect( _representationMenu,
				 &CustomWidget::SetRepresentationMenu ::onRepresentationChange,
				 this,
				 &ContextualMenuMolecule::_applyRepresentationAction );
	}

	void ContextualMenuMolecule::localize() {}

	void ContextualMenuMolecule::setTarget( App::Old::Component::Chemistry::Molecule * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getPdbIdCode() ) );

		const int representationIndex
			= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentationIndex(
				p_target->getRepresentation()->getLinkedRepresentation() );

		_toggleWaterAction->setText( p_target->showWater() ? "Hide waters" : "Show waters" );
		_toggleHydrogenAction->setText( p_target->showHydrogen() ? "Hide hydrogens" : "Show hydrogens" );
		_toggleSolventAction->setText( p_target->showSolvent() ? "Hide solvent" : "Show solvent" );
		_toggleIonAction->setText( p_target->showIon() ? "Hide ions" : "Show ions" );

		_toggleTrajectoryPlayingAction->setVisible( p_target->hasTrajectory() );
		_toggleTrajectoryPlayingAction->setText( p_target->isPlaying() ? "Pause" : "Play" );

		_representationMenu->tickCurrentRepresentation( representationIndex );
	}

	void ContextualMenuMolecule::_renameAction()
	{
		View::UI::Widget::MoleculeSceneView * const molSceneView
			= VTX::MVC_MANAGER().getView<View::UI::Widget::MoleculeSceneView>( _target,
																			   ID::View::UI_MOLECULE_STRUCTURE );

		molSceneView->openRenameEditor( _target->getId() );
	}
	void ContextualMenuMolecule::_toggleWatersVisibilityAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeShowWater( *_target, !_target->showWater() ) );
	}
	void ContextualMenuMolecule::_toggleHydrogensVisibilityAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeShowHydrogen( *_target, !_target->showHydrogen() ) );
	}
	void ContextualMenuMolecule::_toggleSolventVisibilityAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeShowSolvent( *_target, !_target->showSolvent() ) );
	}
	void ContextualMenuMolecule::_toggleIonVisibilityAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeShowIon( *_target, !_target->showIon() ) );
	}

	void ContextualMenuMolecule::_toggleTrajectoryPlayingActions()
	{
		const bool newIsPlaying = !_target->isPlaying();
		VTX_ACTION( new App::Old::Action::Molecule::ChangeIsPlaying( *_target, newIsPlaying ) );
	}

	void ContextualMenuMolecule::_orientAction()
	{
		VTX_ACTION( new VTX::UI::QT::Action::Molecule::Orient( *_target ) );
	}
	void ContextualMenuMolecule::_showAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuMolecule::_hideAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuMolecule::_soloAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::SOLO ) );
	}

	void ContextualMenuMolecule::_copyAction() { VTX_ACTION( new App::Old::Action::Molecule::Copy( *_target ) ); }
	void ContextualMenuMolecule::_deleteAction() { VTX_ACTION( new App::Old::Action::Molecule::Delete( *_target ) ); }

	void ContextualMenuMolecule::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

	void ContextualMenuMolecule::_exportAction() { Dialog::openExportMoleculeDialog(); }
	void ContextualMenuMolecule::_loadTrajectoryAction() { Dialog::openLoadTrajectoryDialog( *_target ); }

} // namespace VTX::UI::Widget::ContextualMenu
