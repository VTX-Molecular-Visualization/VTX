#include "molecule_scene_view.hpp"
#include "molecule_subdata_scene_view.hpp"
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
					QVariantList dataListMolecule;
					dataListMolecule.append( "M" );
					dataListMolecule.append( std::to_string( _model->getId() ).c_str() );
					this->setData( 0, Qt::UserRole, QVariant( dataListMolecule ) );

					// Chains.
					for ( Model::Chain * const chain : _model->getChains() )
					{
						// MoleculeSubDataSceneView * const chainView = new MoleculeSubDataSceneView( *chain, this );
						QTreeWidgetItem * const chainView = new QTreeWidgetItem( this );
						chainView->setText( 0, QString( chain->getName().c_str() ) );
						QVariantList dataListChain;
						dataListChain.append( "C" );
						dataListChain.append( std::to_string( chain->getIndex() ).c_str() );
						chainView->setData( 0, Qt::UserRole, QVariant( dataListChain ) );
						addChild( chainView );
						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							Model::Residue & residue = _model->getResidue( chain->getIndexFirstResidue() + r );
							// const MoleculeSubDataSceneView * const residueView = new MoleculeSubDataSceneView( residue, chainView );
							QTreeWidgetItem * const residueView = new QTreeWidgetItem( chainView );
							residueView->setText( 0, QString( residue.getSymbolStr().c_str() ) );
							QVariantList dataListResidue;
							dataListResidue.append( "R" );
							dataListResidue.append( std::to_string( residue.getIndex() ).c_str() );
							residueView->setData( 0, Qt::UserRole, QVariant( dataListResidue ) );
							chainView->addChild( residueView );
						}
					}
				}

				void MoleculeSceneView::_setupSlots() {}

				void MoleculeSceneView::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
