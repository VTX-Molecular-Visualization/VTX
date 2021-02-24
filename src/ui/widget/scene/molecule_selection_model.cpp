#include "molecule_selection_model.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "selection/selection_manager.hpp"

namespace VTX::UI::Widget::Scene
{
	void MoleculeSelectionModel::select( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command )
	{
		if ( command & QItemSelectionModel::NoUpdate )
		{
			QItemSelectionModel::select( selection, QItemSelectionModel::NoUpdate );
		}
		else
		{
			std::vector<uint> selectionIds = std::vector<uint>();
			_fillVectorWithItemIds( selection, selectionIds );

			Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();

			if ( command & QItemSelectionModel::Select )
			{
				const bool appendToSelection = !( command & QItemSelectionModel::Clear );
				VTX_ACTION( new Action::Selection::SelectModels( selectionModel, selectionIds, appendToSelection ) );
			}
			else if ( command & QItemSelectionModel::Deselect )
			{
				VTX_ACTION( new Action::Selection::UnselectModels( selectionModel, selectionIds ) );
			}
		}
	}
	void MoleculeSelectionModel::select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command )
	{
		if ( command & QItemSelectionModel::NoUpdate )
		{
			QItemSelectionModel::select( index, command );
		}
		else
		{
			const Model::ID &  modelId		  = index.data( Qt::UserRole ).value<VTX::Model::ID>();
			Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();

			if ( command & QItemSelectionModel::Select )
			{
				const bool appendToSelection = !( command & QItemSelectionModel::Clear );
				_selectModelAction( selectionModel, modelId, appendToSelection );
			}
			else if ( command & QItemSelectionModel::Toggle )
			{
				_selectModelAction( selectionModel, modelId, true );
			}
			else if ( command & QItemSelectionModel::Deselect )
			{
				_unselectModelAction( selectionModel, modelId );
			}
		}
	}

	void MoleculeSelectionModel::refreshSelection( const QItemSelection & selection )
	{
		QItemSelectionModel::select( selection, QItemSelectionModel::ClearAndSelect );
	}

	void MoleculeSelectionModel::_fillVectorWithItemIds( const QItemSelection & p_selection,
														 std::vector<uint> &	p_vectorId ) const
	{
		for ( const QItemSelectionRange & modelRange : p_selection )
		{
			p_vectorId.reserve( p_vectorId.size() + modelRange.indexes().size() );

			for ( const QModelIndex & modelIndex : modelRange.indexes() )
			{
				const Model::ID & modelId = modelIndex.data( Qt::UserRole ).value<VTX::Model::ID>();
				p_vectorId.emplace_back( modelId );
			}
		}
	}

	void MoleculeSelectionModel::_selectModelAction( Model::Selection & p_selectionModel,
													 const Model::ID &	p_modelId,
													 const bool			p_appendToSelection ) const
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( p_modelId );

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isMoleculeFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectMolecule( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectMolecule( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isChainFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectChain( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectChain( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isResidueFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectResidue( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectResidue( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isAtomSelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectAtom( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectAtom( p_selectionModel, model, p_appendToSelection ) );
		}
	}

	void MoleculeSelectionModel::_unselectModelAction( Model::Selection & p_selectionModel,
													   const Model::ID &  p_modelId ) const
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( p_modelId );

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectMolecule( p_selectionModel, model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectChain( p_selectionModel, model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectResidue( p_selectionModel, model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectAtom( p_selectionModel, model ) );
		}
	}

} // namespace VTX::UI::Widget::Scene
