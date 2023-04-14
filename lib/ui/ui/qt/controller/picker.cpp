#include "picker.hpp"
#include "qt/action/selection.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/tool/keys.hpp"
#include "qt/tool/render/widget/render_widget.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/selection.hpp>
#include <app/old_app/model/atom.hpp>
#include <app/old_app/model/chain.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/residue.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>
#include <app/old_app/selection/selection_enum.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Controller
{
	Picker::Picker() : _lastClickedIds { Model::ID_UNKNOWN, Model::ID_UNKNOWN } {}
	void Picker::update( const float & p_deltaTime ) { BaseMouseController::update( p_deltaTime ); }

	void Picker::_onMouseLeftClick( const uint p_x, const uint p_y )
	{
		const Vec2i ids = QT_APP()->getMainWindow().getRender()->getPickedIds( p_x, p_y );
		_performSelection( ids );
		_lastClickedIds = ids;
	}

	void Picker::_onMouseRightClick( const uint p_x, const uint p_y )
	{
		QT::Tool::Render::Widget::RenderWidget * renderWidget = QT_APP()->getMainWindow().getRender();

		const Vec2i ids = renderWidget->getPickedIds( p_x, p_y );
		_performSelection( ids );
		_lastClickedIds = ids;

		Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const QPoint position = renderWidget->mapToGlobal( QPoint( p_x, p_y ) );

		if ( selection.isEmpty() )
		{
			QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::RENDER, position );
		}
		else
		{
			QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::SELECTION, &selection, position );
		}
	}

	void Picker::_performSelection( const Vec2i & p_ids ) const
	{
		// Clear selection if not CTRL.
		if ( _isModifierExclusive( ModifierFlag::Control ) == false )
		{
			VTX_ACTION(
				new VTX::Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
		}

		// If something clicked.
		if ( p_ids.x != Model::ID_UNKNOWN )
		{
			const ID::VTX_ID & typeId = MVC::MvcManager::get().getModelTypeID( p_ids.x );

			// Already selected.
			if ( Selection::SelectionManager::get().getSelectionModel().isModelSelected(
					 MVC::MvcManager::get().getModel<Model::BaseModel>( p_ids.x ) )
				 && ( p_ids.y != Model::ID_UNKNOWN
						  ? Selection::SelectionManager::get().getSelectionModel().isModelSelected(
							  MVC::MvcManager::get().getModel<Model::BaseModel>( p_ids.y ) )
						  : true ) )
			{
				// Remove from selection.
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					// Residue.
					if ( typeId == VTX::ID::Model::MODEL_RESIDUE )
					{
						Model::Residue & residuePicked = MVC::MvcManager::get().getModel<Model::Residue>( p_ids.x );
						_unselectItem( residuePicked );
					}
					// Bond.
					else if ( p_ids.y != Model::ID_UNKNOWN )
					{
						Model::Atom & atomPicked1 = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.x );
						Model::Atom & atomPicked2 = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.y );

						_unselectItem( atomPicked1, atomPicked2 );
					}
					// Atom.
					else
					{
						Model::Atom & atomPicked = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.x );
						_unselectItem( atomPicked );
					}
				}
			}
			// Add to selection.
			else
			{
				// Residue.
				if ( typeId == VTX::ID::Model::MODEL_RESIDUE )
				{
					Model::Residue & residuePicked = MVC::MvcManager::get().getModel<Model::Residue>( p_ids.x );
					_selectItem( residuePicked );
				}
				// Bond.
				else if ( p_ids.y != Model::ID_UNKNOWN )
				{
					Model::Atom & atomPicked1 = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.x );
					Model::Atom & atomPicked2 = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.y );

					_selectItem( atomPicked1, atomPicked2 );
				}
				// Atom.
				else
				{
					Model::Atom & atomPicked = MVC::MvcManager::get().getModel<Model::Atom>( p_ids.x );
					_selectItem( atomPicked );
				}
			}
		}
	}

	void Picker::_selectItem( Model::Atom & p_atomPicked ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
			VTX_ACTION( new VTX::Action::Selection::SelectMolecule(
				selectionModel, *p_atomPicked.getMoleculePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::CHAIN:
			VTX_ACTION( new VTX::Action::Selection::SelectChain(
				selectionModel, *p_atomPicked.getChainPtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::RESIDUE:
			VTX_ACTION( new VTX::Action::Selection::SelectResidue(
				selectionModel, *p_atomPicked.getResiduePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::ATOM:
		default:
			VTX_ACTION( new VTX::Action::Selection::SelectAtom( selectionModel, p_atomPicked, appendToSelection ) );
			break;
		}
	}
	void Picker::_selectItem( Model::Atom & p_atomPicked1, Model::Atom & p_atomPicked2 ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
		{
			Model::Molecule * const mol1 = p_atomPicked1.getMoleculePtr();
			Model::Molecule * const mol2 = p_atomPicked2.getMoleculePtr();
			if ( mol1 == mol2 )
			{
				VTX_ACTION( new VTX::Action::Selection::SelectMolecule( selectionModel, *mol1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION(
					new VTX::Action::Selection::SelectMolecule( selectionModel, { mol1, mol2 }, appendToSelection ) );
			}
			break;
		}
		case VTX::Selection::Granularity::CHAIN:
		{
			Model::Chain * const chain1 = p_atomPicked1.getChainPtr();
			Model::Chain * const chain2 = p_atomPicked2.getChainPtr();

			if ( chain1 == chain2 )
			{
				VTX_ACTION( new VTX::Action::Selection::SelectChain( selectionModel, *chain1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION(
					new VTX::Action::Selection::SelectChain( selectionModel, { chain1, chain2 }, appendToSelection ) );
			}
			break;
		}
		case VTX::Selection::Granularity::RESIDUE:
		{
			Model::Residue * const res1 = p_atomPicked1.getResiduePtr();
			Model::Residue * const res2 = p_atomPicked2.getResiduePtr();

			if ( res1 == res2 )
			{
				VTX_ACTION( new VTX::Action::Selection::SelectResidue( selectionModel, *res1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION(
					new VTX::Action::Selection::SelectResidue( selectionModel, { res1, res2 }, appendToSelection ) );
			}
			break;
		}
		case VTX::Selection::Granularity::ATOM:
		default:
		{
			if ( &p_atomPicked1 == &p_atomPicked2 )
			{
				VTX_ACTION(
					new VTX::Action::Selection::SelectAtom( selectionModel, p_atomPicked1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION( new VTX::Action::Selection::SelectAtom(
					selectionModel, { &p_atomPicked1, &p_atomPicked2 }, appendToSelection ) );
			}
			break;
		}
		}
	}
	void Picker::_selectItem( Model::Residue & p_residuePicked ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
			VTX_ACTION( new VTX::Action::Selection::SelectMolecule(
				selectionModel, *p_residuePicked.getMoleculePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::CHAIN:
			VTX_ACTION( new VTX::Action::Selection::SelectChain(
				selectionModel, *p_residuePicked.getChainPtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::RESIDUE:
		case VTX::Selection::Granularity::ATOM:
		default:
			VTX_ACTION(
				new VTX::Action::Selection::SelectResidue( selectionModel, p_residuePicked, appendToSelection ) );
			break;
		}
	}

	void Picker::_unselectItem( Model::Atom & p_atomPicked ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
			VTX_ACTION( new VTX::Action::Selection::UnselectMolecule(
				selectionModel, *p_atomPicked.getMoleculePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::CHAIN:
			VTX_ACTION( new VTX::Action::Selection::UnselectChain(
				selectionModel, *p_atomPicked.getChainPtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::RESIDUE:
			VTX_ACTION( new VTX::Action::Selection::UnselectResidue(
				selectionModel, *p_atomPicked.getResiduePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::ATOM:
		default:
			VTX_ACTION( new VTX::Action::Selection::UnselectAtom( selectionModel, p_atomPicked, appendToSelection ) );
			break;
		}
	}
	void Picker::_unselectItem( Model::Atom & p_atomPicked1, Model::Atom & p_atomPicked2 ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
		{
			Model::Molecule * const mol1 = p_atomPicked1.getMoleculePtr();
			Model::Molecule * const mol2 = p_atomPicked2.getMoleculePtr();
			if ( mol1 == mol2 )
			{
				VTX_ACTION( new VTX::Action::Selection::UnselectMolecule( selectionModel, *mol1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION(
					new VTX::Action::Selection::UnselectMolecule( selectionModel, { mol1, mol2 }, appendToSelection ) );
			}
			break;
		}
		case VTX::Selection::Granularity::CHAIN:
		{
			Model::Chain * const chain1 = p_atomPicked1.getChainPtr();
			Model::Chain * const chain2 = p_atomPicked2.getChainPtr();

			if ( chain1 == chain2 )
			{
				VTX_ACTION( new VTX::Action::Selection::UnselectChain( selectionModel, *chain1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION( new VTX::Action::Selection::UnselectChain(
					selectionModel, { chain1, chain2 }, appendToSelection ) );
			}
			break;
		}
		case VTX::Selection::Granularity::RESIDUE:
		{
			Model::Residue * const res1 = p_atomPicked1.getResiduePtr();
			Model::Residue * const res2 = p_atomPicked2.getResiduePtr();

			if ( res1 == res2 )
			{
				VTX_ACTION( new VTX::Action::Selection::UnselectResidue( selectionModel, *res1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION(
					new VTX::Action::Selection::UnselectResidue( selectionModel, { res1, res2 }, appendToSelection ) );
			}
		}
		break;
		case VTX::Selection::Granularity::ATOM:
		default:
			if ( &p_atomPicked1 == &p_atomPicked2 )
			{
				VTX_ACTION(
					new VTX::Action::Selection::UnselectAtom( selectionModel, p_atomPicked1, appendToSelection ) );
			}
			else
			{
				VTX_ACTION( new VTX::Action::Selection::UnselectAtom(
					selectionModel, { &p_atomPicked1, &p_atomPicked2 }, appendToSelection ) );
			}
			break;
		}
	}
	void Picker::_unselectItem( Model::Residue & p_residuePicked ) const
	{
		Model::Selection & selectionModel	 = Selection::SelectionManager::get().getSelectionModel();
		const bool		   appendToSelection = _isModifierExclusive( ModifierFlag::Control );

		switch ( VTX_SETTING().getSelectionGranularity() )
		{
		case VTX::Selection::Granularity::MOLECULE:
			VTX_ACTION( new VTX::Action::Selection::UnselectMolecule(
				selectionModel, *p_residuePicked.getMoleculePtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::CHAIN:
			VTX_ACTION( new VTX::Action::Selection::UnselectChain(
				selectionModel, *p_residuePicked.getChainPtr(), appendToSelection ) );
			break;
		case VTX::Selection::Granularity::RESIDUE:
		case VTX::Selection::Granularity::ATOM:
		default:
			VTX_ACTION(
				new VTX::Action::Selection::UnselectResidue( selectionModel, p_residuePicked, appendToSelection ) );
			break;
		}
	}

	void Picker::_onMouseLeftDoubleClick( const uint p_x, const uint p_y )
	{
		const Vec2i ids = QT_APP()->getMainWindow().getRender()->getPickedIds( p_x, p_y );

		if ( ids.x == Model::ID_UNKNOWN )
			return;

		if ( _lastClickedIds.x != ids.x || _lastClickedIds.y != ids.y )
			return;

		if ( _isModifierExclusive( ModifierFlag::None ) )
		{
			const Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
			VTX_ACTION( new QT::Action::Selection::Orient( selectionModel ) );
		}
	}

} // namespace VTX::UI::QT::Controller
