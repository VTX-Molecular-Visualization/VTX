#ifndef __VTX_UI_WIDGET_SCENE_ITEM_SELECTION_MODEL__
#define __VTX_UI_WIDGET_SCENE_ITEM_SELECTION_MODEL__

#include "id.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include <QAbstractItemModel>
#include <QItemSelection>
#include <QItemSelectionModel>

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
	class MoleculeSelectionModel : public QItemSelectionModel
	{
	  public:
		MoleculeSelectionModel( const Model::Molecule * const p_molecule,
								QAbstractItemModel * const	  p_model = nullptr ) :
			QItemSelectionModel( p_model ),
			_molecule( p_molecule ) {};

		MoleculeSelectionModel( const Model::Molecule * const p_molecule,
								QAbstractItemModel * const	  p_model,
								QObject * const				  p_parent ) :
			QItemSelectionModel( p_model, p_parent ),
			_molecule( p_molecule ) {};

		void select( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command ) override;
		void select( const QModelIndex & index, QItemSelectionModel::SelectionFlags command ) override;
		void refreshSelection( const QItemSelection & selection );

	  private:
		const Model::Molecule * const _molecule;

		void _selectModelAction( Model::Selection & p_selectionModel,
								 const Model::ID &	p_modelId,
								 const bool			p_appendToSelection ) const;
		void _unselectModelAction( Model::Selection & p_selectionModel, const Model::ID & p_modelId ) const;

		void _fillVectorWithItemIds( const QItemSelection & p_selection, std::vector<uint> & p_vectorId ) const;
		void _selectAllBeforeItemInMolecule( std::vector<uint> &			p_selection,
											 const Model::BaseModel * const p_itemFrom );
		void _selectAllAfterItemInMolecule( std::vector<uint> &			   p_selection,
											const Model::BaseModel * const p_itemFrom );

		void _selectAllChainsFrom( std::vector<uint> & p_selection, const Model::Chain & p_itemFrom );
		void _selectAllResiduesFrom( std::vector<uint> & p_selection, const Model::Residue & p_itemFrom );
		void _selectAllAtomsFrom( std::vector<uint> & p_selection, const Model::Atom & p_itemFrom );

		void _selectAllChainsTo( std::vector<uint> & p_selection, const Model::Chain & p_itemFrom );
		void _selectAllResiduesTo( std::vector<uint> & p_selection, const Model::Residue & p_itemFrom );
		void _selectAllAtomsTo( std::vector<uint> & p_selection, const Model::Atom & p_itemFrom );

		const Model::Molecule * _getMolecule( const Model::BaseModel * const p_model ) const;
		Model::BaseModel &		_getModel( const QModelIndex & p_modelIndex ) const;
	};

} // namespace VTX::UI::Widget::Scene

#endif
