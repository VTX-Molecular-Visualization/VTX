#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM_SELECTION_MODEL__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <app/component/chemistry/_fwd.hpp>
#include <app/core/model/base_model.hpp>
#include <app/application/selection/selection.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class SceneItemWidget;

	class SceneItemSelectionModel : public QItemSelectionModel
	{
	  public:
		SceneItemSelectionModel( const App::Core::Model::BaseModel * const p_baseModel,
								 QAbstractItemModel * const				   p_model = nullptr ) :
			QItemSelectionModel( p_model ),
			_model( p_baseModel ) {};

		SceneItemSelectionModel( const App::Core::Model::BaseModel * const p_baseModel,
								 QAbstractItemModel * const				   p_model,
								 QObject * const						   p_parent ) :
			QItemSelectionModel( p_model, p_parent ),
			_model( p_baseModel ) {};

		void select( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command ) override;
		void select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command ) override;
		void refreshSelection( const QItemSelection & selection );

	  private:
		const App::Core::Model::BaseModel * const _model;

		void _selectModelAction( App::Application::Selection::SelectionModel &			  p_selectionModel,
								 const App::Core::Model::ID & p_modelId,
								 const bool					  p_appendToSelection ) const;
		void _unselectModelAction( App::Application::Selection::SelectionModel & p_selectionModel, const App::Core::Model::ID & p_modelId ) const;

		void _fillVectorWithItemIds( const QItemSelection &				 p_selection,
									 std::vector<App::Core::Model::ID> & p_vectorId ) const;
		void _appendAllSubitemsBeforeObjectInSelectionVector(
			const SceneItemWidget &				p_sceneItemWidget,
			const App::Core::Model::BaseModel & p_itemFrom,
			std::vector<App::Core::Model::ID> & p_selectionVector ) const;
		void _appendAllSubitemsAfterObjectInSelectionVector(
			const SceneItemWidget &				p_sceneItemWidget,
			const App::Core::Model::BaseModel & p_itemFrom,
			std::vector<App::Core::Model::ID> & p_selectionVector ) const;

		App::Core::Model::BaseModel * _getModel( const QModelIndex & p_modelIndex ) const;
		App::Core::Model::BaseModel * _getModel( const SceneItemWidget & p_sceneItem ) const;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
