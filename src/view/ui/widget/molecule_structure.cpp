#include "molecule_structure.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeStructure::MoleculeStructure( Model::Molecule * const p_model, QWidget * p_parent ) :
					BaseManualWidget( p_parent ), View::BaseView<Model::Molecule>( p_model )
				{
				}

				MoleculeStructure::~MoleculeStructure() {}

				void MoleculeStructure::notify( const Event::VTX_EVENT_MODEL & p_event )
				{
					if ( p_event == Event::VTX_EVENT_MODEL::INIT )
					{
						setColumnCount( 1 );
						QTreeWidgetItem * itemMolecule = new QTreeWidgetItem( this, QStringList( QString::fromStdString( _model->getName() ) ) );
						insertTopLevelItem( 0, itemMolecule );

						// Chains.
						for ( const Model::Chain * const chain : _model->getChains() )
						{
							QTreeWidgetItem * itemChain = new QTreeWidgetItem( itemMolecule, QStringList( QString::fromStdString( chain->getName() ) ) );

							// Residues.
							for ( uint r = 0; r < chain->getResidueCount(); ++r )
							{
								const Model::Residue & residue	   = _model->getResidue( chain->getIndexFirstResidue() + r );
								QTreeWidgetItem *	   itemResidue = new QTreeWidgetItem( itemChain, QStringList( QString::fromStdString( residue.getSymbolName() ) ) );
							}
						}
					}
				}

				void MoleculeStructure::setupUi( const QString & p_name ) { BaseManualWidget::setupUi( p_name ); }
				void MoleculeStructure::setupSlots() {}
				void MoleculeStructure::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
