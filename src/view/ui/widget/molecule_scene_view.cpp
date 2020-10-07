#include "molecule_scene_view.hpp"
#include "chain_scene_view.hpp"
#include "residue_scene_view.hpp"
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

					setText( 0, QString::fromStdString( _model->getPdbIdCode() ) );

					// Chains.
					for ( Model::Chain * const chain : _model->getChains() )
					{
						ChainSceneView * chainView = VTX::UI::WidgetFactory::get().GetSceneItem<ChainSceneView, Model::Chain>( chain, this, chain->getName() );

						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							Model::Residue & residue = _model->getResidue( chain->getIndexFirstResidue() + r );
							VTX::UI::WidgetFactory::get().GetSceneItem<ResidueSceneView, Model::Residue>( &residue, chainView, residue.getSymbolName() );
						}
					}
				}
				void MoleculeSceneView::_setupSlots() {}
				void MoleculeSceneView::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
