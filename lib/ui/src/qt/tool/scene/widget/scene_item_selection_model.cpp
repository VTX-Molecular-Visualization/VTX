#include "ui/qt/tool/scene/widget/scene_item_selection_model.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/scene/widget/scene_item_widget.hpp"
#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include <app/action/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/video/path.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	void SceneItemSelectionModel::select( const QItemSelection &			  selection,
										  QItemSelectionModel::SelectionFlags command )
	{
		std::vector<App::Core::Model::ID> selectionIds = std::vector<App::Core::Model::ID>();
		_fillVectorWithItemIds( selection, selectionIds );

		App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

		if ( command == QItemSelectionModel::NoUpdate )
		{
			std::vector<App::Core::Model::ID>::const_iterator it = selectionIds.begin();

			while ( it != selectionIds.end() )
			{
				if ( selectionModel.isModelSelected( *it ) )
					it = selectionIds.erase( it );
				else
					it++;
			}

			if ( selectionIds.size() > 0 )
			{
				VTX_ACTION( new App::Action::Selection::SelectModels( selectionModel, selectionIds, false ) );
			}
			else
			{
				QItemSelectionModel::select( selection, QItemSelectionModel::NoUpdate );
			}
		}
		else
		{
			if ( command & QItemSelectionModel::Current )
			{
				const SceneWidget & sceneWidget
					= *( QT_APP()->getMainWindow().getPanel<SceneWidget>( Tool::SCENE_WINDOW_KEY ) );

				assert( sceneWidget.getSceneItemWidgets().size() > 0 );

				const App::Core::Model::BaseModel * firstSceneItem
					= _getModel( *sceneWidget.getSceneItemWidgets()[ 0 ] );

				if ( selectionModel.getCurrentObject() != nullptr )
				{
					const App::Core::Model::BaseModel * const currentObject = selectionModel.getCurrentObject();
					const App::Core::Model::BaseModel *		  newCurrentSelectedItem
						= currentIndex().isValid() ? _getModel( currentIndex() ) : firstSceneItem;

					const SceneItemWidget * const currentSceneItemObject
						= sceneWidget.getSceneItemWidgetFromModel( *currentObject );
					const SceneItemWidget * const newSceneItemObject
						= sceneWidget.getSceneItemWidgetFromModel( *newCurrentSelectedItem );

					const App::Core::Model::ID & firstObjectId	= currentSceneItemObject->getModelID();
					const App::Core::Model::ID & secondObjectId = newSceneItemObject->getModelID();

					if ( firstObjectId != secondObjectId )
					{
						selectionIds.clear();

						bool startAddToSelection = false;
						for ( const SceneItemWidget * const sceneWidget : sceneWidget.getSceneItemWidgets() )
						{
							const App::Core::Model::ID & itemId = sceneWidget->getModelID();

							if ( itemId == firstObjectId )
							{
								if ( startAddToSelection )
								{
									_appendAllSubitemsBeforeObjectInSelectionVector(
										*currentSceneItemObject, *currentObject, selectionIds );
									break;
								}
								else
								{
									_appendAllSubitemsAfterObjectInSelectionVector(
										*currentSceneItemObject, *currentObject, selectionIds );
									startAddToSelection = true;
								}
							}
							else if ( itemId == secondObjectId )
							{
								if ( startAddToSelection )
								{
									_appendAllSubitemsBeforeObjectInSelectionVector(
										*newSceneItemObject, *newCurrentSelectedItem, selectionIds );
									break;
								}
								else
								{
									_appendAllSubitemsAfterObjectInSelectionVector(
										*newSceneItemObject, *newCurrentSelectedItem, selectionIds );
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
				VTX_ACTION(
					new App::Action::Selection::SelectModels( selectionModel, selectionIds, appendToSelection ) );
			}
			else if ( command & QItemSelectionModel::Deselect )
			{
				VTX_ACTION( new App::Action::Selection::UnselectModels( selectionModel, selectionIds ) );
			}
		}
	}
	void SceneItemSelectionModel::select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command )
	{
		if ( command == QItemSelectionModel::NoUpdate )
		{
			QItemSelectionModel::select( index, command );
		}
		else
		{
			const App::Core::Model::ID & modelId = index.data( Qt::UserRole ).value<VTX::App::Core::Model::ID>();
			App::Application::Selection::SelectionModel & selectionModel
				= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

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

	void SceneItemSelectionModel::_appendAllSubitemsBeforeObjectInSelectionVector(
		const SceneItemWidget &				p_sceneItemWidget,
		const App::Core::Model::BaseModel & p_itemFrom,
		std::vector<App::Core::Model::ID> & p_selectionVector ) const
	{
		const std::vector<App::Core::Model::ID> newItems = p_sceneItemWidget.getAllItemsTo( p_itemFrom );

		const size_t startIndex = p_selectionVector.size();
		p_selectionVector.resize( p_selectionVector.size() + newItems.size() );
		std::move( newItems.begin(), newItems.end(), p_selectionVector.begin() + startIndex );
	}

	void SceneItemSelectionModel::_appendAllSubitemsAfterObjectInSelectionVector(
		const SceneItemWidget &				p_sceneItemWidget,
		const App::Core::Model::BaseModel & p_itemFrom,
		std::vector<App::Core::Model::ID> & p_selectionVector ) const
	{
		const std::vector<App::Core::Model::ID> newItems = p_sceneItemWidget.getAllItemsFrom( p_itemFrom );

		const size_t startIndex = p_selectionVector.size();
		p_selectionVector.resize( p_selectionVector.size() + newItems.size() );
		std::move( newItems.begin(), newItems.end(), p_selectionVector.begin() + startIndex );
	}

	void SceneItemSelectionModel::_fillVectorWithItemIds( const QItemSelection &			  p_selection,
														  std::vector<App::Core::Model::ID> & p_vectorId ) const
	{
		for ( const QItemSelectionRange & modelRange : p_selection )
		{
			p_vectorId.reserve( p_vectorId.size() + modelRange.indexes().size() );

			for ( const QModelIndex & modelIndex : modelRange.indexes() )
			{
				const App::Core::Model::ID & modelId
					= modelIndex.data( Qt::UserRole ).value<VTX::App::Core::Model::ID>();
				p_vectorId.emplace_back( modelId );
			}
		}
	}

	void SceneItemSelectionModel::_selectModelAction( App::Application::Selection::SelectionModel & p_selectionModel,
													  const App::Core::Model::ID &					p_modelId,
													  const bool p_appendToSelection ) const
	{
		const ID::VTX_ID & modelTypeId = VTX::MVC_MANAGER().getModelTypeID( p_modelId );

		if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
		{
			App::Component::Chemistry::Molecule & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isMoleculeFullySelected( model ) )
				VTX_ACTION( new App::Action::Selection::UnselectMolecule( p_selectionModel, model ) );
			else
				VTX_ACTION(
					new App::Action::Selection::SelectMolecule( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CATEGORY )
		{
			App::Component::Chemistry::Category & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isCategoryFullySelected( model ) )
				VTX_ACTION( new App::Action::Selection::UnselectCategory( p_selectionModel, model ) );
			else
				VTX_ACTION(
					new App::Action::Selection::SelectCategory( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
		{
			App::Component::Chemistry::Chain & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isChainFullySelected( model ) )
				VTX_ACTION( new App::Action::Selection::UnselectChain( p_selectionModel, model ) );
			else
				VTX_ACTION( new App::Action::Selection::SelectChain( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
		{
			App::Component::Chemistry::Residue & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isResidueFullySelected( model ) )
				VTX_ACTION( new App::Action::Selection::UnselectResidue( p_selectionModel, model ) );
			else
				VTX_ACTION( new App::Action::Selection::SelectResidue( p_selectionModel, model, p_appendToSelection ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
		{
			App::Component::Chemistry::Atom & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( p_modelId );
			if ( p_appendToSelection && p_selectionModel.isAtomSelected( model ) )
				VTX_ACTION( new App::Action::Selection::UnselectAtom( p_selectionModel, model ) );
			else
				VTX_ACTION( new App::Action::Selection::SelectAtom( p_selectionModel, model, p_appendToSelection ) );
		}
	}

	void SceneItemSelectionModel::_unselectModelAction( App::Application::Selection::SelectionModel & p_selectionModel,
														const App::Core::Model::ID &				  p_modelId ) const
	{
		const ID::VTX_ID & modelTypeId = VTX::MVC_MANAGER().getModelTypeID( p_modelId );

		if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
		{
			App::Component::Chemistry::Molecule & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( p_modelId );
			VTX_ACTION( new App::Action::Selection::UnselectMolecule( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CATEGORY )
		{
			App::Component::Chemistry::Category & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( p_modelId );
			VTX_ACTION( new App::Action::Selection::UnselectCategory( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
		{
			App::Component::Chemistry::Chain & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( p_modelId );
			VTX_ACTION( new App::Action::Selection::UnselectChain( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
		{
			App::Component::Chemistry::Residue & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( p_modelId );
			VTX_ACTION( new App::Action::Selection::UnselectResidue( p_selectionModel, model ) );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
		{
			App::Component::Chemistry::Atom & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( p_modelId );
			VTX_ACTION( new App::Action::Selection::UnselectAtom( p_selectionModel, model ) );
		}
	}

	App::Core::Model::BaseModel * SceneItemSelectionModel::_getModel( const QModelIndex & p_modelIndex ) const
	{
		const VTX::App::Core::Model::ID & modelId
			= p_modelIndex.data( SceneItemWidget::MODEL_ID_ROLE ).value<VTX::App::Core::Model::ID>();
		App::Core::Model::BaseModel & model = VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( modelId );

		return &model;
	}
	App::Core::Model::BaseModel * SceneItemSelectionModel::_getModel( const SceneItemWidget & p_sceneItem ) const
	{
		const VTX::App::Core::Model::ID & modelId = p_sceneItem.getModelID();
		App::Core::Model::BaseModel &	  model	  = VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( modelId );

		return &model;
	}

} // namespace VTX::UI::QT::Tool::Scene::Widget
