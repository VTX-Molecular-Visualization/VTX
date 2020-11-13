#include "selection_view.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
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
				void SelectionView::notify( const Event::VTXEvent * const p_event ) { BaseView::notify( p_event ); }

				void SelectionView::_setupUi( const QString & p_name )
				{
					setObjectName( QString::fromUtf8( "selectionTree" ) );
					setColumnCount( 1 );
					setHeaderHidden( true );

					_refreshView();
				}

				void SelectionView::_setupSlots() { connect( this, &QTreeWidget::itemClicked, this, &SelectionView::_onItemClicked ); }

				void SelectionView::_refreshView()
				{
					_clear();

					const Model::Selection::MapMoleculeIds & items = _model->getItems();
					for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & pairMolecule : items )
					{
						const Model::Molecule & molecule	 = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
						QTreeWidgetItem * const moleculeView = new QTreeWidgetItem( this );
						moleculeView->setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( pairMolecule.first ) );
						moleculeView->setText( 0, QString::fromStdString( molecule.getDefaultName() ) );

						// Chains.
						for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : pairMolecule.second )
						{
							const Model::Chain &	chain	  = molecule.getChain( pairChain.first );
							QTreeWidgetItem * const chainView = new QTreeWidgetItem( moleculeView );
							chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain.getId() ) );
							chainView->setText( 0, QString::fromStdString( chain.getDefaultName() ) );

							// Residues.
							for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
							{
								const Model::Residue &	residue		= molecule.getResidue( pairResidue.first );
								QTreeWidgetItem * const residueView = new QTreeWidgetItem( chainView );
								residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
								residueView->setText( 0, QString::fromStdString( residue.getSymbolStr() + " " + std::to_string( residue.getIndex() ) ) );

								// Atom.
								for ( const uint & a : pairResidue.second )
								{
									const Model::Atom &		atom	 = molecule.getAtom( a );
									QTreeWidgetItem * const atomView = new QTreeWidgetItem( residueView );
									atomView->setData( 0, Qt::UserRole, QVariant::fromValue( atom.getId() ) );
									atomView->setText( 0, QString::fromStdString( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) ) );
									residueView->addChild( atomView );
								}

								chainView->addChild( residueView );
							}
							moleculeView->addChild( chainView );
						}
						addTopLevelItem( moleculeView );
					}
				}

				void SelectionView::_clear()
				{
					for ( uint i = 0; i < uint( topLevelItemCount() ); ++i )
					{
						QTreeWidgetItem * const c = topLevelItem( i );
						delete c;
					}
				}

				void SelectionView::_onItemClicked( QTreeWidgetItem * p_item, int p_column )
				{
					const Model::ID &  modelId		  = _getModelID( *p_item );
					ID::VTX_ID		   modelTypeId	  = MVC::MvcManager::get().getModelTypeID( modelId );
					Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

					if ( modelTypeId == ID::Model::MODEL_MOLECULE )
					{
						const Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
						VTX_ACTION( new Action::Selection::UnselectMolecule( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_CHAIN )
					{
						const Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
						VTX_ACTION( new Action::Selection::UnselectChain( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
					{
						const Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
						VTX_ACTION( new Action::Selection::UnselectResidue( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_ATOM )
					{
						const Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
						VTX_ACTION( new Action::Selection::UnselectAtom( selectionModel, model ) );
					}
				}

				void SelectionView::localize() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
