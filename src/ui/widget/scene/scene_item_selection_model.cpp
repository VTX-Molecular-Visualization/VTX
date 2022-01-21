#include "scene_item_selection_model.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/path.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::Scene
{
	void SceneItemSelectionModel::select( const QItemSelection &			  selection,
										  QItemSelectionModel::SelectionFlags command )
	{
		if ( command & QItemSelectionModel::NoUpdate )
		{
			QItemSelectionModel::select( selection, QItemSelectionModel::NoUpdate );
		}
		else
		{
			std::vector<uint> selectionIds = std::vector<uint>();
			_fillVectorWithItemIds( selection, selectionIds );

			Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

			if ( command & QItemSelectionModel::Current )
			{
				const SceneWidget & sceneWidget
					= VTXApp::get().getMainWindow().getWidget<SceneWidget>( ID::UI::Window::SCENE );

				assert( sceneWidget.getSceneItemWidgets().size() > 0 );

				const Model::BaseModel * firstSceneItem = _getModel( *sceneWidget.getSceneItemWidgets()[ 0 ] );

				if ( selectionModel.getCurrentObject() != nullptr )
				{
					const Model::BaseModel * const currentObject = selectionModel.getCurrentObject();
					const Model::BaseModel *	   newCurrentSelectedItem
						= currentIndex().isValid() ? _getModel( currentIndex() ) : firstSceneItem;

					// TODO manage this to include all SceneItemObjects types
					const Model::BaseModel * const moleculeCurrentObject = _getTopLevelItem( currentObject );
					const Model::BaseModel * const moleculeNewCurrentObject
						= _getTopLevelItem( newCurrentSelectedItem );

					const Model::ID & firstMoleculeId  = moleculeCurrentObject->getId();
					const Model::ID & secondMoleculeId = moleculeNewCurrentObject->getId();

					if ( firstMoleculeId != secondMoleculeId )
					{
						selectionIds.clear();

						bool startAddToSelection = false;
						for ( const SceneItemWidget * const sceneWidget : sceneWidget.getSceneItemWidgets() )
						{
							const Model::ID & itemId = sceneWidget->getModelID();

							if ( itemId == firstMoleculeId )
							{
								if ( startAddToSelection )
								{
									_selectAllBeforeItemInMolecule( selectionIds, currentObject );
									break;
								}
								else
								{
									_selectAllAfterItemInMolecule( selectionIds, currentObject );
									startAddToSelection = true;
								}
							}
							else if ( itemId == secondMoleculeId )
							{
								if ( startAddToSelection )
								{
									_selectAllBeforeItemInMolecule( selectionIds, newCurrentSelectedItem );
									break;
								}
								else
								{
									_selectAllAfterItemInMolecule( selectionIds, newCurrentSelectedItem );
									startAddToSelection = true;
								}
							}
							else if ( startAddToSelection )
							{
								selectionIds.emplace_back( itemId );
							}
						}
					}
				}
			}

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
	void SceneItemSelectionModel::select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command )
	{
		if ( command & QItemSelectionModel::NoUpdate )
		{
			QItemSelectionModel::select( index, command );
		}
		else
		{
			const Model::ID &  modelId		  = index.data( Qt::UserRole ).value<VTX::Model::ID>();
			Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

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

	void SceneItemSelectionModel::refreshSelection( const QItemSelection & selection )
	{
		QItemSelectionModel::select( selection, QItemSelectionModel::ClearAndSelect );
	}

	void SceneItemSelectionModel::_selectAllAfterItemInMolecule( std::vector<uint> &			p_selection,
																 const Model::BaseModel * const p_itemFrom )
	{
		if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_CHAIN )
		{
			const Model::Chain * const chainFrom = static_cast<const Model::Chain *>( p_itemFrom );
			p_selection.emplace_back( chainFrom->getId() );
			_selectAllChainsFrom( p_selection, *chainFrom );
		}
		else if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_RESIDUE )
		{
			const Model::Residue * const residueFrom = static_cast<const Model::Residue *>( p_itemFrom );
			p_selection.emplace_back( residueFrom->getId() );
			_selectAllResiduesFrom( p_selection, *residueFrom );
			_selectAllChainsFrom( p_selection, *residueFrom->getChainPtr() );
		}
		else if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_ATOM )
		{
			const Model::Atom * const atomFrom = static_cast<const Model::Atom *>( p_itemFrom );
			p_selection.emplace_back( atomFrom->getId() );
			_selectAllAtomsFrom( p_selection, *atomFrom );
			_selectAllResiduesFrom( p_selection, *atomFrom->getResiduePtr() );
			_selectAllChainsFrom( p_selection, *atomFrom->getChainPtr() );
		}
		else // Molecule and others
		{
			p_selection.emplace_back( p_itemFrom->getId() );
		}
	}
	void SceneItemSelectionModel::_selectAllChainsFrom( std::vector<uint> &	 p_selection,
														const Model::Chain & p_itemFrom )
	{
		const Model::Molecule & molecule = *( p_itemFrom.getMoleculePtr() );

		for ( uint iChain = p_itemFrom.getIndex() + 1; iChain < molecule.getChainCount(); iChain++ )
			p_selection.emplace_back( molecule.getChain( iChain )->getId() );
	}
	void SceneItemSelectionModel::_selectAllResiduesFrom( std::vector<uint> &	 p_selection,
														  const Model::Residue & p_itemFrom )
	{
		const Model::Chain &	chain	 = *( p_itemFrom.getChainPtr() );
		const Model::Molecule & molecule = *( chain.getMoleculePtr() );

		for ( uint iResidue = p_itemFrom.getIndex() + 1; iResidue <= chain.getIndexLastResidue(); iResidue++ )
		{
			p_selection.emplace_back( molecule.getResidue( iResidue )->getId() );
		}
	}
	void SceneItemSelectionModel::_selectAllAtomsFrom( std::vector<uint> & p_selection, const Model::Atom & p_itemFrom )
	{
		const Model::Residue &	residue	 = *( p_itemFrom.getResiduePtr() );
		const Model::Molecule & molecule = *( residue.getMoleculePtr() );

		for ( uint iAtom = p_itemFrom.getIndex() + 1; iAtom < residue.getIndexFirstAtom() + residue.getAtomCount();
			  iAtom++ )
		{
			p_selection.emplace_back( molecule.getAtom( iAtom )->getId() );
		}
	}

	void SceneItemSelectionModel::_selectAllBeforeItemInMolecule( std::vector<uint> &			 p_selection,
																  const Model::BaseModel * const p_itemFrom )
	{
		if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_CHAIN )
		{
			const Model::Chain * const chainFrom = static_cast<const Model::Chain *>( p_itemFrom );
			p_selection.emplace_back( chainFrom->getId() );
			_selectAllChainsTo( p_selection, *chainFrom );
		}
		else if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_RESIDUE )
		{
			const Model::Residue * const residueFrom = static_cast<const Model::Residue *>( p_itemFrom );
			p_selection.emplace_back( residueFrom->getId() );
			_selectAllResiduesTo( p_selection, *residueFrom );
			_selectAllChainsTo( p_selection, *residueFrom->getChainPtr() );
		}
		else if ( p_itemFrom->getTypeId() == VTX::ID::Model::MODEL_ATOM )
		{
			const Model::Atom * const atomFrom = static_cast<const Model::Atom *>( p_itemFrom );
			p_selection.emplace_back( atomFrom->getId() );
			_selectAllAtomsTo( p_selection, *atomFrom );
			_selectAllResiduesTo( p_selection, *atomFrom->getResiduePtr() );
			_selectAllChainsTo( p_selection, *atomFrom->getChainPtr() );
		}
		else
		{
			p_selection.emplace_back( p_itemFrom->getId() );
		}
	}
	void SceneItemSelectionModel::_selectAllChainsTo( std::vector<uint> & p_selection, const Model::Chain & p_itemFrom )
	{
		const Model::Molecule & molecule = *( p_itemFrom.getMoleculePtr() );

		for ( uint iChain = 0; iChain < p_itemFrom.getIndex(); iChain++ )
			p_selection.emplace_back( molecule.getChain( iChain )->getId() );
	}
	void SceneItemSelectionModel::_selectAllResiduesTo( std::vector<uint> &	   p_selection,
														const Model::Residue & p_itemFrom )
	{
		const Model::Chain &	chain	 = *( p_itemFrom.getChainPtr() );
		const Model::Molecule & molecule = *( chain.getMoleculePtr() );

		for ( uint iResidue = chain.getIndexFirstResidue(); iResidue < p_itemFrom.getIndex(); iResidue++ )
			p_selection.emplace_back( molecule.getResidue( iResidue )->getId() );
	}
	void SceneItemSelectionModel::_selectAllAtomsTo( std::vector<uint> & p_selection, const Model::Atom & p_itemFrom )
	{
		const Model::Residue &	residue	 = *( p_itemFrom.getResiduePtr() );
		const Model::Molecule & molecule = *( residue.getMoleculePtr() );

		for ( uint iAtom = residue.getIndexFirstAtom(); iAtom < p_itemFrom.getIndex(); iAtom++ )
			p_selection.emplace_back( molecule.getAtom( iAtom )->getId() );
	}

	const Model::BaseModel * SceneItemSelectionModel::_getTopLevelItem( const Model::BaseModel * const p_model ) const
	{
		const Model::BaseModel * res;

		if ( p_model->getTypeId() == VTX::ID::Model::MODEL_CHAIN )
			res = static_cast<const Model::Chain *>( p_model )->getMoleculePtr();
		else if ( p_model->getTypeId() == VTX::ID::Model::MODEL_RESIDUE )
			res = static_cast<const Model::Residue *>( p_model )->getMoleculePtr();
		else if ( p_model->getTypeId() == VTX::ID::Model::MODEL_ATOM )
			res = static_cast<const Model::Atom *>( p_model )->getMoleculePtr();
		else if ( p_model->getTypeId() == VTX::ID::Model::MODEL_PATH )
			res = p_model;
		else if ( p_model->getTypeId() == VTX::ID::Model::MODEL_VIEWPOINT )
			res = static_cast<const Model::Viewpoint *>( p_model )->getPathPtr();
		else // Molecule / Labels, etc
			res = p_model;

		return res;
	}

	void SceneItemSelectionModel::_fillVectorWithItemIds( const QItemSelection & p_selection,
														  std::vector<uint> &	 p_vectorId ) const
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

	void SceneItemSelectionModel::_selectModelAction( Model::Selection & p_selectionModel,
													  const Model::ID &	 p_modelId,
													  const bool		 p_appendToSelection ) const
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( p_modelId );

		if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isMoleculeFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectMolecule( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectMolecule( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isChainFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectChain( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectChain( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isResidueFullySelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectResidue( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectResidue( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isAtomSelected( model ) )
				VTX_ACTION( new Action::Selection::UnselectAtom( p_selectionModel, model ) );
			else
				VTX_ACTION( new Action::Selection::SelectAtom( p_selectionModel, model, p_appendToSelection ) );
		}
	}

	void SceneItemSelectionModel::_unselectModelAction( Model::Selection & p_selectionModel,
														const Model::ID &  p_modelId ) const
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( p_modelId );

		if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectMolecule( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectChain( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectResidue( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( p_modelId );
			VTX_ACTION( new Action::Selection::UnselectAtom( p_selectionModel, model ) );
		}
	}

	Model::BaseModel * SceneItemSelectionModel::_getModel( const QModelIndex & p_modelIndex ) const
	{
		const VTX::Model::ID & modelId = p_modelIndex.data( SceneItemWidget::MODEL_ID_ROLE ).value<VTX::Model::ID>();
		Model::BaseModel &	   model   = MVC::MvcManager::get().getModel<Model::BaseModel>( modelId );

		return &model;
	}
	Model::BaseModel * SceneItemSelectionModel::_getModel( const SceneItemWidget & p_sceneItem ) const
	{
		const VTX::Model::ID & modelId = p_sceneItem.getModelID();
		Model::BaseModel &	   model   = MVC::MvcManager::get().getModel<Model::BaseModel>( modelId );

		return &model;
	}

} // namespace VTX::UI::Widget::Scene
