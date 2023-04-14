#ifndef __VTX_UI_WIDGET_SCENE_ITEM_SELECTION_MODEL__
#define __VTX_UI_WIDGET_SCENE_ITEM_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <app/old_app/id.hpp>
#include <app/old_app/model/base_model.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/selection.hpp>

namespace VTX::Model
{
	class Molecule;
	class Chain;
	class Molecule;
	class Residue;
	class Atom;
} // namespace VTX::Model

namespace VTX::UI::Widget::Scene
{
	class SceneItemWidget;

	class SceneItemSelectionModel : public QItemSelectionModel
	{
	  public:
		SceneItemSelectionModel( const Model::BaseModel * const p_baseModel,
								 QAbstractItemModel * const		p_model = nullptr ) :
			QItemSelectionModel( p_model ),
			_model( p_baseModel ) {};

		SceneItemSelectionModel( const Model::BaseModel * const p_baseModel,
								 QAbstractItemModel * const		p_model,
								 QObject * const				p_parent ) :
			QItemSelectionModel( p_model, p_parent ),
			_model( p_baseModel ) {};

		void select( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command ) override;
		void select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command ) override;
		void refreshSelection( const QItemSelection & selection );

	  private:
		const Model::BaseModel * const _model;

		void _selectModelAction( Model::Selection & p_selectionModel,
								 const Model::ID &	p_modelId,
								 const bool			p_appendToSelection ) const;
		void _unselectModelAction( Model::Selection & p_selectionModel, const Model::ID & p_modelId ) const;

		void _fillVectorWithItemIds( const QItemSelection & p_selection, std::vector<Model::ID> & p_vectorId ) const;
		void _appendAllSubitemsBeforeObjectInSelectionVector( const SceneItemWidget &  p_sceneItemWidget,
															  const Model::BaseModel & p_itemFrom,
															  std::vector<Model::ID> & p_selectionVector ) const;
		void _appendAllSubitemsAfterObjectInSelectionVector( const SceneItemWidget &  p_sceneItemWidget,
															 const Model::BaseModel & p_itemFrom,
															 std::vector<Model::ID> & p_selectionVector ) const;

		Model::BaseModel * _getModel( const QModelIndex & p_modelIndex ) const;
		Model::BaseModel * _getModel( const SceneItemWidget & p_sceneItem ) const;
	};

} // namespace VTX::UI::Widget::Scene

#endif
