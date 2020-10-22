#include "molecule_scene_view.hpp"
#include "molecule_subdata_scene_view.hpp"
#include "ui/widget/scene_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeSceneView::MoleculeSceneView( Model::Molecule * const p_model, QTreeWidgetItem * p_parent ) : BaseSceneItem<Model::Molecule>( p_model, p_parent ) {}

				void MoleculeSceneView::_setupUi( const QString & p_name )
				{
					BaseSceneItem<Model::Molecule>::_setupUi( p_name );

					// Chains.
					for ( Model::Chain * const chain : _model->getChains() )
					{
						MoleculeSubDataSceneView * const chainView = new MoleculeSubDataSceneView( *chain, this );

						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							Model::Residue &					   residue	   = _model->getResidue( chain->getIndexFirstResidue() + r );
							const MoleculeSubDataSceneView * const residueView = new MoleculeSubDataSceneView( residue, chainView );
						}
					}
				}
				void MoleculeSceneView::_setupSlots() {}
				void MoleculeSceneView::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
