#include "contextual_menu_selection.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "action/visible.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"
#include <QTimer>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuSelection::ContextualMenuSelection( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent )
	{
		_actions.emplace_back(
			new ActionData( "Rename", TypeMask::Molecule, this, &ContextualMenuSelection::_renameAction ) );

		_actions.emplace_back( new ActionDataSection( "Representation", TypeMask::AllButAtom, this ) );
		_representationMenu = WidgetFactory::get().instantiateWidget<CustomWidget::SetRepresentationMenu>(
			this, "SetRepresentationMenu" );
		_actions.emplace_back( new SubMenuData( "Representation", TypeMask::AllButAtom, this, _representationMenu ) );
		_actions.emplace_back( new ActionDataSection( "Show/Hide", TypeMask::Molecule, this ) );

		ActionData * const toggleWatersAction = new ActionData(
			"Toggle Waters", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleWaterVisibilityAction );
		toggleWatersAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleWaterText );
		_actions.emplace_back( toggleWatersAction );

		ActionData * const toggleHydogensAction = new ActionData(
			"Toggle Hydrogens", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleHydrogenVisibilityAction );
		toggleHydogensAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleHydrogenText );
		_actions.emplace_back( toggleHydogensAction );

		ActionData * const toggleSolventAction = new ActionData(
			"Toggle Solvent", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleSolventVisibilityAction );
		toggleSolventAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleSolventText );
		_actions.emplace_back( toggleSolventAction );

		ActionData * const toggleIonAction = new ActionData(
			"Toggle Ions", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleIonVisibilityAction );
		toggleIonAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleIonText );
		_actions.emplace_back( toggleIonAction );

		_actions.emplace_back( new ActionDataSection( "Trajectory", TypeMask::Molecule, this ) );
		ActionData * const toggleTrajectoryPlayAction = new ActionData(
			"Toggle Playing", TypeMask::Molecule, this, &ContextualMenuSelection::_toggleTrajectoryPlayingAction );

		toggleTrajectoryPlayAction->setRefreshFunction( &ContextualMenuSelection ::_refreshToggleTrajectoryPlay );
		toggleTrajectoryPlayAction->setCheckFunction( &ContextualMenuSelection ::_checkToggleTrajectoryPlayAction );
		_actions.emplace_back( toggleTrajectoryPlayAction );

		_actions.emplace_back( new ActionDataSection( "Edit", TypeMask::All, this ) );
		_actions.emplace_back(
			new ActionData( "Orient", TypeMask::All, this, &ContextualMenuSelection::_orientAction ) );
		_actions.emplace_back(
			new ActionData( "Show", TypeMask::AllButAtom, this, &ContextualMenuSelection::_showAction ) );
		_actions.emplace_back(
			new ActionData( "Hide", TypeMask::AllButAtom, this, &ContextualMenuSelection::_hideAction ) );
		_actions.emplace_back(
			new ActionData( "Duplicate", TypeMask::All, this, &ContextualMenuSelection::_copyAction ) );
		_actions.emplace_back(
			new ActionData( "Extract", TypeMask::AllButMolecule, this, &ContextualMenuSelection::_extractAction ) );
		_actions.emplace_back(
			new ActionData( "Delete", TypeMask::All, this, &ContextualMenuSelection::_deleteAction ) );

		_actions.emplace_back( new ActionDataSection( "Export", TypeMask::Molecule, this ) );
		_actions.emplace_back(
			new ActionData( "Export", TypeMask::Molecule, this, &ContextualMenuSelection::_exportAction ) );
	}
	ContextualMenuSelection ::~ContextualMenuSelection()
	{
		for ( int i = 0; i < _actions.size(); i++ )
		{
			delete ( _actions[ i ] );
		}

		_actions.clear();
	}

	void ContextualMenuSelection::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuSelection::_setupSlots()
	{
		for ( ItemData * const itemData : _actions )
		{
			itemData->appendToMenu( this );
		}

		connect( _representationMenu,
				 &CustomWidget::SetRepresentationMenu::onRepresentationChange,
				 this,
				 &ContextualMenuSelection::_applyRepresentationAction );

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

		TypeMask selectionTypeMask = _getTypeMaskFromTypeSet( typesInSelection );

		for ( int i = 0; i < _actions.size(); i++ )
		{
			ItemData * const itemData	   = _actions[ i ];
			const bool		 actionVisible = bool( itemData->validTypes & selectionTypeMask ) && itemData->check();

			if ( actionVisible )
				itemData->refresh();

			actions()[ i ]->setVisible( actionVisible );
		}

		_updateCurrentRepresentationFeedback();
	}

	TypeMask ContextualMenuSelection::_getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds )
	{
		TypeMask res = TypeMask::None;

		if ( p_typeIds.find( ID::Model::MODEL_MOLECULE ) != p_typeIds.end() )
			res |= TypeMask::Molecule;
		if ( p_typeIds.find( ID::Model::MODEL_CHAIN ) != p_typeIds.end() )
			res |= TypeMask::Chain;
		if ( p_typeIds.find( ID::Model::MODEL_RESIDUE ) != p_typeIds.end() )
			res |= TypeMask::Residue;
		if ( p_typeIds.find( ID::Model::MODEL_ATOM ) != p_typeIds.end() )
			res |= TypeMask::Atom;

		return res;
	}

	void ContextualMenuSelection::_renameAction()
	{
		const Model::ID &		moleculeId = _target->getItems().begin()->first;
		const Model::Molecule & molecule   = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeId );

		View::UI::Widget::MoleculeSceneView & moleculeSceneView
			= *MVC::MvcManager::get().getView<View::UI::Widget::MoleculeSceneView>( &molecule,
																					ID::View::UI_MOLECULE_STRUCTURE );

		moleculeSceneView.openRenameEditor();
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

			if ( focusedModelTypeID == ID::Model::MODEL_MOLECULE )
				focusedModelBaseVisible = static_cast<Model::Molecule *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_CHAIN )
				focusedModelBaseVisible = static_cast<Model::Chain *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_RESIDUE )
				focusedModelBaseVisible = static_cast<Model::Residue *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_ATOM )
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

			if ( focusedModelTypeID == ID::Model::MODEL_MOLECULE )
				focusedModelBaseVisible = static_cast<Model::Molecule *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_CHAIN )
				focusedModelBaseVisible = static_cast<Model::Chain *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_RESIDUE )
				focusedModelBaseVisible = static_cast<Model::Residue *>( _focusedTarget );
			else if ( focusedModelTypeID == ID::Model::MODEL_ATOM )
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
	void ContextualMenuSelection::_copyAction() { VTX_ACTION( new Action::Selection::Copy( *_target ) ); }
	void ContextualMenuSelection::_extractAction() { VTX_ACTION( new Action::Selection::Extract( *_target ) ); }
	void ContextualMenuSelection::_deleteAction() { VTX_ACTION( new Action::Selection::Delete( *_target ) ); }
	void ContextualMenuSelection::_exportAction() { UI::Dialog::openExportMoleculeDialog(); }

	void ContextualMenuSelection::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new Action::Selection::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

	void ContextualMenuSelection::_updateCurrentRepresentationFeedback()
	{
		bool allSelectionHasSameRepresentation = true;
		int	 selectionRepresentationIndex	   = -1;

		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
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
				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : moleculeData.second )
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
						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueData :
							  chainData.second )
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

		if ( allSelectionHasSameRepresentation )
			_representationMenu->tickCurrentRepresentation( selectionRepresentationIndex );
		else
			_representationMenu->removeTick();
	}

	void ContextualMenuSelection::_refreshToggleWaterText( QAction & _action ) const
	{
		bool displayShowWater = true;
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
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
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
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
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
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
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			displayShowIon			   = displayShowIon && !molecule.showIon();
		}

		QString text = displayShowIon ? "Show ions" : "Hide ions";
		_action.setText( text );
	}

	void ContextualMenuSelection::_refreshToggleTrajectoryPlay( QAction & _action ) const
	{
		bool displayPlay = true;
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
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
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeData : _target->getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
			if ( molecule.hasTrajectory() )
				return true;
		}

		return false;
	}

} // namespace VTX::UI::Widget::ContextualMenu
