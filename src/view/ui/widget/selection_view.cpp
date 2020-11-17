#include "selection_view.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "tool/chrono.hpp"
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
				void SelectionView::_setupUi( const QString & p_name )
				{
					setObjectName( QString::fromUtf8( "selectionTree" ) );
					setColumnCount( 1 );
					setHeaderHidden( true );
				}

				void SelectionView::_setupSlots() { connect( this, &QTreeWidget::itemClicked, this, &SelectionView::_onItemClicked ); }

				void SelectionView::_refreshView()
				{
					Tool::Chrono chrono = Tool::Chrono();
					chrono.start();

					blockSignals( true );
					setUpdatesEnabled( false );

					const Model::Selection::MapMoleculeIds & items = _model->getItems();
					uint									 m	   = 0;
					_createTopLevelChildren( uint( items.size() ) );
					for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & pairMolecule : items )
					{
						const Model::Molecule & molecule	 = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
						QTreeWidgetItem * const moleculeView = topLevelItem( m++ );
						moleculeView->setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( pairMolecule.first ) );
						moleculeView->setText( 0, QString::fromStdString( molecule.getDefaultName() ) );

						// Chains.
						uint c = 0;
						_createChildren( *moleculeView, uint( pairMolecule.second.size() ) );
						for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : pairMolecule.second )
						{
							const Model::Chain &	chain	  = molecule.getChain( pairChain.first );
							QTreeWidgetItem * const chainView = moleculeView->child( c++ );
							chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain.getId() ) );
							chainView->setText( 0, QString::fromStdString( chain.getDefaultName() ) );

							// Residues.
							uint r = 0;
							_createChildren( *chainView, uint( pairChain.second.size() ) );
							for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
							{
								const Model::Residue &	residue		= molecule.getResidue( pairResidue.first );
								QTreeWidgetItem * const residueView = chainView->child( r++ );
								residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
								residueView->setText( 0, QString::fromStdString( residue.getSymbolStr() + " " + std::to_string( residue.getIndex() ) ) );

								// Atom.
								uint a = 0;
								_createChildren( *residueView, uint( pairResidue.second.size() ) );
								for ( const uint & atomIndex : pairResidue.second )
								{
									const Model::Atom &		atom	 = molecule.getAtom( atomIndex );
									QTreeWidgetItem * const atomView = residueView->child( a++ );
									atomView->setData( 0, Qt::UserRole, QVariant::fromValue( atom.getId() ) );
									atomView->setText( 0, QString::fromStdString( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) ) );
								}
							}
						}
					}

					blockSignals( false );
					setUpdatesEnabled( true );

					chrono.stop();
					VTX_INFO( "Selection UI time: " + std::to_string( chrono.elapsedTime() ) );
				}

				void SelectionView::_createTopLevelChildren( const uint p_count )
				{
					while ( uint( topLevelItemCount() ) < p_count )
					{
						addTopLevelItem( new QTreeWidgetItem() );
					}
					while ( uint( topLevelItemCount() ) > p_count )
					{
						delete topLevelItem( topLevelItemCount() - 1 );
					}
				}

				void SelectionView::_createChildren( QTreeWidgetItem & p_item, const uint p_count )
				{
					while ( uint( p_item.childCount() ) < p_count )
					{
						p_item.addChild( new QTreeWidgetItem() );
					}
					while ( uint( p_item.childCount() ) > p_count )
					{
						delete p_item.child( p_item.childCount() - 1 );
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
