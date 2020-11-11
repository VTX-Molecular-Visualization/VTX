#include "selection_view.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				void SelectionView::notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData )
				{
					BaseView::notify( p_event, p_eventData );
				}

				void SelectionView::_setupUi( const QString & p_name ) { refreshView(); }

				void SelectionView::_setupSlots() {}

				void SelectionView::refreshView()
				{
					clear();

					setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( _model->getId() ) );
					setText( 0, QString::fromStdString( "Selection #1" ) );

					const Model::Selection::MapMoleculeIds & items = _model->getItems();
					for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & pairMolecule : items )
					{
						const Model::Molecule & molecule	 = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
						QTreeWidgetItem * const moleculeView = new QTreeWidgetItem( this );
						moleculeView->setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( pairMolecule.first ) );
						moleculeView->setText( 0, QString::fromStdString( molecule.getDefaultName() ) );
						addChild( moleculeView );

						// Chains.
						for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : pairMolecule.second )
						{
							const Model::Chain &	chain	  = molecule.getChain( pairChain.first );
							QTreeWidgetItem * const chainView = new QTreeWidgetItem( moleculeView );
							chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain.getId() ) );
							chainView->setText( 0, QString::fromStdString( chain.getDefaultName() ) );
							moleculeView->addChild( chainView );

							// Residues.
							for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
							{
								const Model::Residue &	residue		= molecule.getResidue( pairResidue.first );
								QTreeWidgetItem * const residueView = new QTreeWidgetItem( chainView );
								residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
								residueView->setText( 0, QString::fromStdString( residue.getSymbolStr() + " " + std::to_string( residue.getIndex() ) ) );
								chainView->addChild( residueView );

								// Atom.
								for ( const uint & a : pairResidue.second )
								{
									const Model::Atom &		atom	 = molecule.getAtom( a );
									QTreeWidgetItem * const atomView = new QTreeWidgetItem( residueView );
									atomView->setData( 0, Qt::UserRole, QVariant::fromValue( atom.getId() ) );
									atomView->setText( 0, QString::fromStdString( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) ) );
									residueView->addChild( atomView );
								}
							}
						}
					}
				}

				void SelectionView::clear()
				{
					for ( uint i = 0; i < uint( childCount() ); ++i )
					{
						QTreeWidgetItem * const c = child( i );
						removeChild( c );
						delete c;
					}
				}

				void SelectionView::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
