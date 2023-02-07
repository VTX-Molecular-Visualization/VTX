#include "contextual_menu_selection.hpp"
// #include "__new_archi/tool/analysis/rmsd/action.hpp"
//  #include "__new_archi/tool/analysis/structural_alignment/action.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/tool/keys.hpp"
#include "qt/tool/scene/widget/scene_widget.hpp"
#include "qt/tool/session/dialog.hpp"
#include <QTimer>
#include <old/action/action_manager.hpp>
#include <old/action/label.hpp>
#include <old/action/selection.hpp>
#include <old/action/viewpoint.hpp>
#include <old/action/visible.hpp>
#include <old/model/generated_molecule.hpp>
#include <old/model/representation/representation_library.hpp>
#include <string>

namespace VTX::UI::QT::Widget::ContextualMenu
{
	ContextualMenuSelection::ContextualMenuSelection( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent )
	{
		SelectionSubMenu * const moleculeStructureSubmenu = new SelectionSubMenu( this, "Molecule" );

		moleculeStructureSubmenu->addItemData(
			new ActionData( "Rename", TypeMask::Molecule, this, &ContextualMenuSelection::_renameAction ) );

		moleculeStructureSubmenu->addItemData( new ActionDataSection( "Representation", TypeMask::AllButAtom, this ) );
		//_representationMenu
		//	= QT::WidgetFactory::get().instantiateWidget<UI::Widget::CustomWidget::SetRepresentationMenu>(
		//		this, "SetRepresentationMenu" );

		// SubMenuData * const changeRepresentationSubmenuData
		//	= new SubMenuData( "Representation", TypeMask::AllButAtom, this, _representationMenu );
		// changeRepresentationSubmenuData->setRefreshFunction(
		//	&ContextualMenuSelection::_updateCurrentRepresentationFeedback );
		// moleculeStructureSubmenu->addItemData( changeRepresentationSubmenuData );

		moleculeStructureSubmenu->addItemData( new ActionDataSection( "Show/Hide", TypeMask::Molecule, this ) );
		ActionData * const toggleWatersAction = new ActionData(
			"Toggle Waters", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleWaterVisibilityAction );
		toggleWatersAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleWaterText );
		moleculeStructureSubmenu->addItemData( toggleWatersAction );

		ActionData * const toggleHydogensAction = new ActionData(
			"Toggle Hydrogens", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleHydrogenVisibilityAction );
		toggleHydogensAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleHydrogenText );
		moleculeStructureSubmenu->addItemData( toggleHydogensAction );

		ActionData * const toggleSolventAction = new ActionData(
			"Toggle Solvent", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleSolventVisibilityAction );
		toggleSolventAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleSolventText );
		moleculeStructureSubmenu->addItemData( toggleSolventAction );

		ActionData * const toggleIonAction = new ActionData(
			"Toggle Ions", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleIonVisibilityAction );
		toggleIonAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleIonText );
		moleculeStructureSubmenu->addItemData( toggleIonAction );

		moleculeStructureSubmenu->addItemData( new ActionDataSection( "Trajectory", TypeMask::Molecule, this ) );
		ActionData * const toggleTrajectoryPlayAction = new ActionData(
			"Toggle Playing", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleTrajectoryPlayingAction );

		toggleTrajectoryPlayAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleTrajectoryPlay );
		toggleTrajectoryPlayAction->setCheckFunction( &ContextualMenuSelection ::_checkToggleTrajectoryPlayAction );
		moleculeStructureSubmenu->addItemData( toggleTrajectoryPlayAction );

		moleculeStructureSubmenu->addItemData( new ActionData(
			"Load Trajectory", TypeMask::Molecule, this, &ContextualMenuSelection::_loadTrajectoryAction ) );

		moleculeStructureSubmenu->addItemData( new ActionDataSection( "Edit", TypeMask::All, this ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Orient", TypeMask::MoleculeStructure, this, &ContextualMenuSelection::_orientAction ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Show", TypeMask::MoleculeStructure, this, &ContextualMenuSelection::_showAction ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Hide", TypeMask::MoleculeStructure, this, &ContextualMenuSelection::_hideAction ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Solo", TypeMask::MoleculeStructure, this, &ContextualMenuSelection::_soloAction ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Duplicate", TypeMask::MoleculeStructure, this, &ContextualMenuSelection::_copyAction ) );

		//_frameListMenu = QT::WidgetFactory::get().instantiateWidget<UI::Widget::CustomWidget::TrajectoryFramesMenu>(
		//	this, "frameListMenu" );
		//_frameListMenu->setDisplayAllFramesOption( true );
		// connect( _frameListMenu,
		//		 &UI::Widget::CustomWidget::TrajectoryFramesMenu::onFrameSelected,
		//		 this,
		//		 &ContextualMenuSelection::_copyFrameAction );
		// SubMenuData * const duplicateFrameSubmenu
		//	= new SubMenuData( "Duplicate Frame", TypeMask::MoleculeStructure, this, _frameListMenu );
		// duplicateFrameSubmenu->setRefreshFunction( &ContextualMenuSelection::_refreshFrameListMenuItems );
		// moleculeStructureSubmenu->addItemData( duplicateFrameSubmenu );

		moleculeStructureSubmenu->addItemData(
			new ActionData( "Extract", TypeMask::AllButMolecule, this, &ContextualMenuSelection::_extractAction ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Delete", TypeMask::All, this, &ContextualMenuSelection::_deleteAction ) );

		moleculeStructureSubmenu->addItemData( new ActionDataSection( "Export", TypeMask::Molecule, this ) );
		moleculeStructureSubmenu->addItemData(
			new ActionData( "Export", TypeMask::Molecule, this, &ContextualMenuSelection::_exportAction ) );

		// moleculeStructureSubmenu->addItemData( new ActionDataSection( "Analysis", TypeMask::MoleculeStructure, this )
		// ); ActionData * const applyComputeRMSDAction = new ActionData( 	"RMSD", TypeMask::MoleculeStructure, this,
		//&ContextualMenuSelection::_applyComputeRMSDAction ); applyComputeRMSDAction->setCheckFunction(
		// &ContextualMenuSelection::_checkComputeRMSDAction ); moleculeStructureSubmenu->addItemData(
		// applyComputeRMSDAction ); ActionData * const applyAlignmentAction 	= new ActionData( "Align",
		// TypeMask::Molecule, this, &ContextualMenuSelection::_applyAlignmentAction );
		// applyAlignmentAction->setCheckFunction( &ContextualMenuSelection::_checkApplyAlignementAction );
		// moleculeStructureSubmenu->addItemData( applyAlignmentAction );
		// moleculeStructureSubmenu->addItemData( new ActionData(
		//	"Alignment settings", TypeMask::Molecule, this, &ContextualMenuSelection::_openAlignmentWindowAction ) );

		// VIEWPOINTS //////////////////////////////////////////////////////////////////////////////////////////////////
		SelectionSubMenu * const viewpointSubmenu = new SelectionSubMenu( this, "Viewpoint" );
		viewpointSubmenu->addItemData(
			new ActionData( "Go to", TypeMask::Viewpoint, this, &ContextualMenuSelection::_gotoViewpointAction ) );
		viewpointSubmenu->addItemData( new ActionDataSection( "Edit", TypeMask::Viewpoint, this ) );
		viewpointSubmenu->addItemData(
			new ActionData( "Rename", TypeMask::Viewpoint, this, &ContextualMenuSelection::_renameAction ) );
		viewpointSubmenu->addItemData( new ActionData(
			"Relocate", TypeMask::Viewpoint, this, &ContextualMenuSelection::_relocateViewpointAction ) );
		viewpointSubmenu->addItemData(
			new ActionData( "Delete", TypeMask::Viewpoint, this, &ContextualMenuSelection::_deleteViewpointAction ) );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// LABELS //////////////////////////////////////////////////////////////////////////////////////////////////
		SelectionSubMenu * const labelSubmenu = new SelectionSubMenu( this, "Label" );
		labelSubmenu->addItemData(
			new ActionData( "Orient", TypeMask::Label, this, &ContextualMenuSelection::_orientToLabelAction ) );
		labelSubmenu->addItemData( new ActionDataSection( "Edit", TypeMask::Label, this ) );
		labelSubmenu->addItemData(
			new ActionData( "Rename", TypeMask::Label, this, &ContextualMenuSelection::_renameAction ) );
		labelSubmenu->addItemData(
			new ActionData( "Delete", TypeMask::Label, this, &ContextualMenuSelection::_deleteLabelAction ) );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		_submenus.resize( int( SUBMENU_TEMPLATE::COUNT ) );
		_submenus[ int( SUBMENU_TEMPLATE::MOLECULE_STRUCTURE ) ] = moleculeStructureSubmenu;
		_submenus[ int( SUBMENU_TEMPLATE::VIEWPOINT ) ]			 = viewpointSubmenu;
		_submenus[ int( SUBMENU_TEMPLATE::LABEL ) ]				 = labelSubmenu;

		_submenusMap[ VTX::ID::Model::MODEL_MOLECULE ] = int( SUBMENU_TEMPLATE::MOLECULE_STRUCTURE );
		_submenusMap[ VTX::ID::Model::MODEL_CHAIN ]	   = int( SUBMENU_TEMPLATE::MOLECULE_STRUCTURE );
		_submenusMap[ VTX::ID::Model::MODEL_RESIDUE ]  = int( SUBMENU_TEMPLATE::MOLECULE_STRUCTURE );
		_submenusMap[ VTX::ID::Model::MODEL_ATOM ]	   = int( SUBMENU_TEMPLATE::MOLECULE_STRUCTURE );

		_submenusMap[ VTX::ID::Model::MODEL_VIEWPOINT ] = int( SUBMENU_TEMPLATE::VIEWPOINT );
		_submenusMap[ VTX::ID::Model::MODEL_PATH ]		= int( SUBMENU_TEMPLATE::VIEWPOINT );

		_submenusMap[ VTX::ID::Model::MODEL_LABEL ]							= int( SUBMENU_TEMPLATE::LABEL );
		_submenusMap[ VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE ]			= int( SUBMENU_TEMPLATE::LABEL );
		_submenusMap[ VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE ] = int( SUBMENU_TEMPLATE::LABEL );
		_submenusMap[ VTX::ID::Model::MODEL_MEASUREMENT_ANGLE ]				= int( SUBMENU_TEMPLATE::LABEL );
		_submenusMap[ VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE ]	= int( SUBMENU_TEMPLATE::LABEL );
	}
	ContextualMenuSelection ::~ContextualMenuSelection()
	{
		for ( SelectionSubMenu * const submenu : _submenus )
			delete submenu;
	}

	void ContextualMenuSelection::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuSelection::_setupSlots()
	{
		// connect( _representationMenu,
		//		 &UI::Widget::CustomWidget::SetRepresentationMenu::onRepresentationChange,
		//		 this,
		//		 &ContextualMenuSelection::_applyRepresentationAction );

		connect(
			this, &ContextualMenuSelection::aboutToShow, this, &ContextualMenuSelection::_updateActionsWithSelection );
	}

	void ContextualMenuSelection::localize() {}

	void ContextualMenuSelection::hideEvent( QHideEvent * p_event )
	{
		ContextualMenuTemplate::hideEvent( p_event );

		// Delayed action because hide call before launching QAction
		QTimer::singleShot( 0, this, [ this ] { _focusedTarget = nullptr; } );
	}

	void ContextualMenuSelection::setFocusedTarget( Model::BaseModel * const p_focusedTarget )
	{
		_focusedTarget = p_focusedTarget;
	}

	void ContextualMenuSelection::_updateActionsWithSelection()
	{
		std::set<ID::VTX_ID> typesInSelection = std::set<ID::VTX_ID>();
		_target->getItemTypes( typesInSelection );

		const TypeMask selectionTypeMask = _getTypeMaskFromTypeSet( typesInSelection );

		std::set<SelectionSubMenu *> submenuDisplayed = std::set<SelectionSubMenu *>();
		for ( const ID::VTX_ID & itemType : typesInSelection )
		{
			submenuDisplayed.emplace( _submenus[ _submenusMap[ itemType ] ] );
		}

		const bool actionsInSubmenu = submenuDisplayed.size() > 1;

		clear();

		for ( SelectionSubMenu * const submenu : submenuDisplayed )
		{
			submenu->getMenu()->clear();
			if ( actionsInSubmenu )
			{
				submenu->refreshWithTarget( *_target, selectionTypeMask, submenu->getMenu() );
				addMenu( submenu->getMenu() );
			}
			else
			{
				submenu->refreshWithTarget( *_target, selectionTypeMask, this );
			}
		}
	}

	TypeMask ContextualMenuSelection::_getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds )
	{
		TypeMask res = TypeMask::None;

		if ( p_typeIds.find( VTX::ID::Model::MODEL_MOLECULE ) != p_typeIds.end() )
			res |= TypeMask::Molecule;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_CHAIN ) != p_typeIds.end() )
			res |= TypeMask::Chain;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_RESIDUE ) != p_typeIds.end() )
			res |= TypeMask::Residue;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_ATOM ) != p_typeIds.end() )
			res |= TypeMask::Atom;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_VIEWPOINT ) != p_typeIds.end() )
			res |= TypeMask::Viewpoint;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_PATH ) != p_typeIds.end() )
			res |= TypeMask::Viewpoint;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_LABEL ) != p_typeIds.end() )
			res |= TypeMask::Label;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE ) != p_typeIds.end() )
			res |= TypeMask::Label;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE ) != p_typeIds.end() )
			res |= TypeMask::Label;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_MEASUREMENT_ANGLE ) != p_typeIds.end() )
			res |= TypeMask::Label;
		if ( p_typeIds.find( VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE ) != p_typeIds.end() )
			res |= TypeMask::Label;

		return res;
	}

	void ContextualMenuSelection::_renameAction()
	{
		const Model::ID & modelID = *_target->getItems().begin();

		const Tool::Scene::Widget::SceneWidget * const sceneWidget
			= QT_APP()->getMainWindow().getPanel<Tool::Scene::Widget::SceneWidget>( QT::Tool::SCENE_WINDOW_KEY );

		if ( sceneWidget != nullptr )
			sceneWidget->openRenameEditor( modelID );
	}

	void ContextualMenuSelection::_toggleWaterVisibilityAction()
	{
		VTX_ACTION( new Action::Selection::ToggleWatersVisibility( *_target ) );
	}
	void ContextualMenuSelection::_toggleHydrogenVisibilityAction()
	{
		VTX_ACTION( new Action::Selection::ToggleHydrogensVisibility( *_target ) );
	}
	void ContextualMenuSelection::_toggleSolventVisibilityAction()
	{
		VTX_ACTION( new Action::Selection::ToggleSolventVisibility( *_target ) );
	}
	void ContextualMenuSelection::_toggleIonVisibilityAction()
	{
		VTX_ACTION( new Action::Selection::ToggleIonsVisibility( *_target ) );
	}

	void ContextualMenuSelection::_toggleTrajectoryPlayingAction()
	{
		VTX_ACTION( new Action::Selection::ToggleTrajectoryPlaying( *_target ) );
	}

	void ContextualMenuSelection::_orientAction() { VTX_ACTION( new Action::Selection::Orient( *_target ) ); }
	void ContextualMenuSelection::_showAction()
	{
		if ( _focusedTarget != nullptr )
		{
			const ID::VTX_ID &	   focusedModelTypeID	   = _focusedTarget->getTypeId();
			Generic::BaseVisible * focusedModelBaseVisible = nullptr;

			if ( focusedModelTypeID == VTX::ID::Model::MODEL_MOLECULE )
				focusedModelBaseVisible = static_cast<Model::Molecule *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CATEGORY )
				focusedModelBaseVisible = static_cast<Model::Category *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CHAIN )
				focusedModelBaseVisible = static_cast<Model::Chain *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_RESIDUE )
				focusedModelBaseVisible = static_cast<Model::Residue *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_ATOM )
				focusedModelBaseVisible = static_cast<Model::Atom *>( _focusedTarget );

			if ( focusedModelBaseVisible != nullptr )
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target,
					*focusedModelBaseVisible,
					focusedModelTypeID,
					Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
			}
			else
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
			}
		}
		else
		{
			VTX_ACTION( new Action::Selection::ChangeVisibility(
				*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
		}
	}
	void ContextualMenuSelection::_hideAction()
	{
		if ( _focusedTarget != nullptr )
		{
			const ID::VTX_ID &	   focusedModelTypeID	   = _focusedTarget->getTypeId();
			Generic::BaseVisible * focusedModelBaseVisible = nullptr;

			if ( focusedModelTypeID == VTX::ID::Model::MODEL_MOLECULE )
				focusedModelBaseVisible = static_cast<Model::Molecule *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CATEGORY )
				focusedModelBaseVisible = static_cast<Model::Category *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CHAIN )
				focusedModelBaseVisible = static_cast<Model::Chain *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_RESIDUE )
				focusedModelBaseVisible = static_cast<Model::Residue *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_ATOM )
				focusedModelBaseVisible = static_cast<Model::Atom *>( _focusedTarget );

			if ( focusedModelBaseVisible != nullptr )
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target,
					*focusedModelBaseVisible,
					focusedModelTypeID,
					Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
			}
			else
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
			}
		}
		else
		{
			VTX_ACTION( new Action::Selection::ChangeVisibility(
				*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
		}
	}
	void ContextualMenuSelection::_soloAction()
	{
		if ( _focusedTarget != nullptr )
		{
			const ID::VTX_ID &	   focusedModelTypeID	   = _focusedTarget->getTypeId();
			Generic::BaseVisible * focusedModelBaseVisible = nullptr;

			if ( focusedModelTypeID == VTX::ID::Model::MODEL_MOLECULE )
				focusedModelBaseVisible = static_cast<Model::Molecule *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CATEGORY )
				focusedModelBaseVisible = static_cast<Model::Category *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_CHAIN )
				focusedModelBaseVisible = static_cast<Model::Chain *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_RESIDUE )
				focusedModelBaseVisible = static_cast<Model::Residue *>( _focusedTarget );
			else if ( focusedModelTypeID == VTX::ID::Model::MODEL_ATOM )
				focusedModelBaseVisible = static_cast<Model::Atom *>( _focusedTarget );

			if ( focusedModelBaseVisible != nullptr )
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target,
					*focusedModelBaseVisible,
					focusedModelTypeID,
					Action::Visible::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
			}
			else
			{
				VTX_ACTION( new Action::Selection::ChangeVisibility(
					*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
			}
		}
		else
		{
			VTX_ACTION( new Action::Selection::ChangeVisibility(
				*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
		}
	}
	void ContextualMenuSelection::_copyAction() { VTX_ACTION( new Action::Selection::Copy( *_target ) ); }
	void ContextualMenuSelection::_copyFrameAction( const int p_frame )
	{
		VTX_ACTION( new Action::Selection::Copy( *_target, p_frame ) );
	}

	void ContextualMenuSelection::_extractAction() { VTX_ACTION( new Action::Selection::Extract( *_target ) ); }
	void ContextualMenuSelection::_deleteAction() { VTX_ACTION( new Action::Selection::Delete( *_target ) ); }
	void ContextualMenuSelection::_exportAction() { QT::Tool::Session::Dialog::openExportMoleculeDialog(); }
	void ContextualMenuSelection::_loadTrajectoryAction()
	{
		Model::Molecule * molecule = nullptr;

		if ( _focusedTarget->getTypeId() == VTX::ID::Model::MODEL_MOLECULE )
		{
			molecule = static_cast<Model::Molecule *>( _focusedTarget );
		}
		else if ( _target->getMoleculesMap().size() > 0 )
		{
			const Model::ID & moleculeID = _target->getMoleculesMap().begin()->first;
			molecule					 = &( MVC::MvcManager::get().getModel<Model::Molecule>( moleculeID ) );
		}

		if ( molecule != nullptr )
		{
			QT::Tool::Session::Dialog::openLoadTrajectoryDialog( *molecule );
		}
	}

	void ContextualMenuSelection::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new Action::Selection::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

	void ContextualMenuSelection::_updateCurrentRepresentationFeedback( QAction & _action ) const
	{
		bool allSelectionHasSameRepresentation = true;
		int	 selectionRepresentationIndex	   = -1;

		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );

			if ( moleculeData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
			{
				const int representationIndex
					= Model::Representation::RepresentationLibrary::get().getRepresentationIndex(
						molecule.getRepresentation()->getLinkedRepresentation() );

				if ( selectionRepresentationIndex == -1 )
					selectionRepresentationIndex = representationIndex;

				allSelectionHasSameRepresentation = selectionRepresentationIndex == representationIndex;
			}
			else
			{
				for ( const Model::Selection::PairChainIds & chainData : moleculeData.second )
				{
					Model::Chain * const chain = molecule.getChain( chainData.first );

					if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
					{
						const int representationIndex
							= Model::Representation::RepresentationLibrary::get().getRepresentationIndex(
								chain->getRepresentation()->getLinkedRepresentation() );

						if ( selectionRepresentationIndex == -1 )
							selectionRepresentationIndex = representationIndex;

						allSelectionHasSameRepresentation = selectionRepresentationIndex == representationIndex;
					}
					else
					{
						for ( const Model::Selection::PairResidueIds & residueData : chainData.second )
						{
							Model::Residue * const residue = molecule.getResidue( residueData.first );

							const int representationIndex
								= Model::Representation::RepresentationLibrary::get().getRepresentationIndex(
									residue->getRepresentation()->getLinkedRepresentation() );

							if ( selectionRepresentationIndex == -1 )
								selectionRepresentationIndex = representationIndex;

							allSelectionHasSameRepresentation = selectionRepresentationIndex == representationIndex;

							if ( !allSelectionHasSameRepresentation )
								break;
						}
					}

					if ( !allSelectionHasSameRepresentation )
						break;
				}
			}

			if ( !allSelectionHasSameRepresentation )
				break;
		}

		// if ( allSelectionHasSameRepresentation )
		//	_representationMenu->tickCurrentRepresentation( selectionRepresentationIndex );
		// else
		//	_representationMenu->removeTick();
	}

	void ContextualMenuSelection::_refreshToggleWaterText( QAction & _action ) const
	{
		bool displayShowWater = true;
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			displayShowWater		   = displayShowWater && !molecule.showWater();
		}

		QString text = displayShowWater ? "Show waters" : "Hide waters";
		_action.setText( text );
	}
	void ContextualMenuSelection::_refreshToggleHydrogenText( QAction & _action ) const
	{
		bool displayShowHydrogen = true;
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			displayShowHydrogen		   = displayShowHydrogen && !molecule.showHydrogen();
		}

		QString text = displayShowHydrogen ? "Show hydrogens" : "Hide hydrogens";
		_action.setText( text );
	}
	void ContextualMenuSelection::_refreshToggleSolventText( QAction & _action ) const
	{
		bool displayShowSolvent = true;
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			displayShowSolvent		   = displayShowSolvent && !molecule.showSolvent();
		}

		QString text = displayShowSolvent ? "Show solvent" : "Hide solvent";
		_action.setText( text );
	}
	void ContextualMenuSelection::_refreshToggleIonText( QAction & _action ) const
	{
		bool displayShowIon = true;
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			displayShowIon			   = displayShowIon && !molecule.showIon();
		}

		QString text = displayShowIon ? "Show ions" : "Hide ions";
		_action.setText( text );
	}

	void ContextualMenuSelection::_refreshFrameListMenuItems( QAction & _action ) const
	{
		//_frameListMenu->updateFrames( *_target );
		//_action.setVisible( _frameListMenu->getFrameCount() >= 2 );
	}

	void ContextualMenuSelection::_refreshToggleTrajectoryPlay( QAction & _action ) const
	{
		bool displayPlay = true;
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			if ( molecule.hasTrajectory() )
				displayPlay = displayPlay && !molecule.isPlaying();
		}

		QString text = displayPlay ? "Play" : "Pause";
		_action.setText( text );
	}

	bool ContextualMenuSelection::_checkToggleTrajectoryPlayAction() const
	{
		for ( const Model::Selection::PairMoleculeIds & moleculeData : _target->getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			if ( molecule.hasTrajectory() )
				return true;
		}

		return false;
	}

	void ContextualMenuSelection::_gotoViewpointAction()
	{
		const std::vector<Model::Viewpoint *> viewpointsInSelection
			= _target->getItemsOfType<Model::Viewpoint>( VTX::ID::Model::MODEL_VIEWPOINT );

		if ( viewpointsInSelection.size() > 0 )
			VTX_ACTION( new Action::Viewpoint::GoTo( **viewpointsInSelection.crbegin() ) );
	}
	void ContextualMenuSelection::_relocateViewpointAction()
	{
		std::vector<Model::Viewpoint *> viewpointsInSelection
			= _target->getItemsOfType<Model::Viewpoint>( VTX::ID::Model::MODEL_VIEWPOINT );

		VTX_ACTION( new Action::Viewpoint::Relocate( viewpointsInSelection ) );
	}
	void ContextualMenuSelection::_deleteViewpointAction()
	{
		VTX_ACTION( new Action::Viewpoint::Delete(
			_target->getItemsOfType<Model::Viewpoint>( VTX::ID::Model::MODEL_VIEWPOINT ) ) );
	}

	void ContextualMenuSelection::_orientToLabelAction()
	{
		std::unordered_set<Model::Label *> labelsInSelection = std::unordered_set<Model::Label *>();
		_getAllLabelTypes( labelsInSelection );

		if ( labelsInSelection.size() > 0 )
			VTX_ACTION( new Action::Label::Orient( labelsInSelection ) );
	}
	void ContextualMenuSelection::_deleteLabelAction()
	{
		std::unordered_set<Model::Label *> labelsInSelection = std::unordered_set<Model::Label *>();
		_getAllLabelTypes( labelsInSelection );

		if ( labelsInSelection.size() > 0 )
			VTX_ACTION( new Action::Label::Delete( labelsInSelection ) );
	}

	void ContextualMenuSelection::_getAllLabelTypes( std::unordered_set<Model::Label *> & p_labels ) const
	{
		_target->getItemsOfType<Model::Label>( VTX::ID::Model::MODEL_LABEL, p_labels );
		_target->getItemsOfType<Model::Label>( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE, p_labels );
		_target->getItemsOfType<Model::Label>( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE, p_labels );
		_target->getItemsOfType<Model::Label>( VTX::ID::Model::MODEL_MEASUREMENT_ANGLE, p_labels );
		_target->getItemsOfType<Model::Label>( VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE, p_labels );
	}

	// bool ContextualMenuSelection::_checkComputeRMSDAction() const
	//{
	//	std::vector<Model::Molecule *> molecules
	//		= _target->getItemsOfType<Model::Molecule>( ID::Model::MODEL_MOLECULE );

	//	return molecules.size() >= 2;
	//}
	// bool ContextualMenuSelection::_checkApplyAlignementAction() const
	//{
	//	std::vector<Model::Molecule *> molecules
	//		= _target->getItemsOfType<Model::Molecule>( ID::Model::MODEL_MOLECULE );

	//	return molecules.size() >= 2;
	//}

	// void ContextualMenuSelection::_applyComputeRMSDAction()
	//{
	//	VTX_ACTION( new VTX::Tool::Analysis::RMSD::Action::ComputeRMSD( *_target ) );
	// }

	// void ContextualMenuSelection::_applyAlignmentAction()
	//{
	//	VTX_ACTION( new VTX::Tool::Analysis::StructuralAlignment::Action::ComputeStructuralAlignment( *_target ) );
	// }

	// void ContextualMenuSelection::_openAlignmentWindowAction()
	//{
	//	VTXApp::get().getMainWindow().showWidget( ID::UI::Window::STRUCTURAL_ALIGNMENT, true );
	// }

} // namespace VTX::UI::QT::Widget::ContextualMenu
