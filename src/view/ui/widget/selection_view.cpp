#include "selection_view.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QHeaderView>

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
					setColumnCount( 2 );
					setHeaderHidden( true );
					setSelectionMode( QAbstractItemView::SelectionMode::NoSelection );
					setExpandsOnDoubleClick( false );

					this->setItemDelegateForColumn( REMOVE_COLUMN_INDEX, new SelectionStyleItemDelegate() );

					header()->setStretchLastSection( false );
					header()->setMinimumSectionSize( 10 );
					header()->setSectionResizeMode( NAME_COLUMN_INDEX, QHeaderView::ResizeMode::Stretch );
					header()->setSectionResizeMode( REMOVE_COLUMN_INDEX, QHeaderView::ResizeMode::ResizeToContents );
				}

				void SelectionView::_setupSlots() { connect( this, &QTreeWidget::itemClicked, this, &SelectionView::_onItemClicked ); }

				void SelectionView::_refreshView()
				{
					Tool::Chrono chrono = Tool::Chrono();
					chrono.start();

					blockSignals( true );
					setUpdatesEnabled( false );

					const Model::Selection::MapMoleculeIds & items				= _model->getItems();
					uint									 m					= 0;
					const bool								 needUpdateMolecule = _createTopLevelChildren( uint( items.size() ) );
					for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & pairMolecule : items )
					{
						QTreeWidgetItem * const moleculeView = topLevelItem( m++ );

						const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );

						// Chains.
						uint	   c			   = 0;
						const bool needUpdateChain = _createChildren( *moleculeView, uint( pairMolecule.second.size() ) );
						for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : pairMolecule.second )
						{
							QTreeWidgetItem * const chainView = moleculeView->child( c++ );

							// Residues.
							uint	   r				 = 0;
							const bool needUpdateResidue = _createChildren( *chainView, uint( pairChain.second.size() ) );
							for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
							{
								QTreeWidgetItem * const residueView = chainView->child( r++ );

								// Atom.
								uint	   a			  = 0;
								const bool needUpdateAtom = _createChildren( *residueView, uint( pairResidue.second.size() ) );
								if ( needUpdateAtom )
								{
									for ( const uint & atomIndex : pairResidue.second )
									{
										const Model::Atom &		atom	 = molecule.getAtom( atomIndex );
										QTreeWidgetItem * const atomView = residueView->child( a++ );

										atomView->setData( NAME_COLUMN_INDEX, Qt::UserRole, QVariant::fromValue( atom.getId() ) );
										atomView->setText( NAME_COLUMN_INDEX, QString::fromStdString( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) ) );
									}
								}

								if ( needUpdateResidue == false )
								{
									continue;
								}
								const Model::Residue & residue = molecule.getResidue( pairResidue.first );
								residueView->setExpanded( false );
								residueView->setData( NAME_COLUMN_INDEX, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
								residueView->setText( NAME_COLUMN_INDEX,
													  QString::fromStdString( residue.getSymbolStr() + " " + std::to_string( residue.getIndexInOriginalChain() ) ) );
							}

							if ( needUpdateChain == false )
							{
								continue;
							}
							const Model::Chain & chain = molecule.getChain( pairChain.first );
							chainView->setExpanded( false );
							chainView->setData( NAME_COLUMN_INDEX, Qt::UserRole, QVariant::fromValue( chain.getId() ) );
							chainView->setText( NAME_COLUMN_INDEX, QString::fromStdString( chain.getDefaultName() ) );
						}

						if ( needUpdateMolecule == false )
						{
							continue;
						}
						moleculeView->setExpanded( false );
						moleculeView->setData( NAME_COLUMN_INDEX, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( pairMolecule.first ) );
						moleculeView->setText( NAME_COLUMN_INDEX, QString::fromStdString( molecule.getDefaultName() ) );
					}

					blockSignals( false );
					setUpdatesEnabled( true );

					chrono.stop();
					VTX_INFO( "Selection UI time: " + std::to_string( chrono.elapsedTime() ) );
				}

				bool SelectionView::_createTopLevelChildren( const uint p_count )
				{
					if ( uint( topLevelItemCount() ) == p_count )
					{
						return false;
					}
					while ( uint( topLevelItemCount() ) < p_count )
					{
						addTopLevelItem( new QTreeWidgetItem() );
					}
					while ( uint( topLevelItemCount() ) > p_count )
					{
						delete topLevelItem( topLevelItemCount() - 1 );
					}
					return true;
				}

				bool SelectionView::_createChildren( QTreeWidgetItem & p_item, const uint p_count )
				{
					if ( uint( p_item.childCount() ) == p_count )
					{
						return false;
					}
					while ( uint( p_item.childCount() ) < p_count )
					{
						p_item.addChild( new QTreeWidgetItem() );
					}
					while ( uint( p_item.childCount() ) > p_count )
					{
						delete p_item.child( p_item.childCount() - 1 );
					}
					return true;
				}

				void SelectionView::_onItemClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
				{
					if ( p_column == REMOVE_COLUMN_INDEX )
					{
						const Model::ID &  modelId		  = _getModelID( *p_item );
						ID::VTX_ID		   modelTypeId	  = MVC::MvcManager::get().getModelTypeID( modelId );
						Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

						if ( modelTypeId == ID::Model::MODEL_MOLECULE )
						{
							Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
							VTX_ACTION( new Action::Selection::UnselectMolecule( selectionModel, model ) );
						}
						else if ( modelTypeId == ID::Model::MODEL_CHAIN )
						{
							Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
							VTX_ACTION( new Action::Selection::UnselectChain( selectionModel, model ) );
						}
						else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
						{
							Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
							VTX_ACTION( new Action::Selection::UnselectResidue( selectionModel, model ) );
						}
						else if ( modelTypeId == ID::Model::MODEL_ATOM )
						{
							Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
							VTX_ACTION( new Action::Selection::UnselectAtom( selectionModel, model ) );
						}
					}
				}

				void SelectionView::localize() {}


				// /////////////////////  SelectionStyleItemDelegate  ///////////////////////////
				void SelectionView::SelectionStyleItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
				{
					if ( option.state & QStyle::State_MouseOver )
						painter->fillRect( option.rect, option.palette.highlight() );

					const int minColumnBorder = ( option.rect.width() < option.rect.height() ? option.rect.width() : option.rect.height() );
					const int size			  = ( minColumnBorder < _getSize() ? minColumnBorder : _getSize() ) - 2;

					const QRect rect = QRect( option.rect.x() + ( ( option.rect.width() - size ) / 2 ), option.rect.y() + ( ( option.rect.height() - size ) / 2 ), size, size );
					painter->drawPixmap( rect, Style::IconConst::get().CLOSE_PIXMAP );
				}

				QSize SelectionView::SelectionStyleItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
				{
					const int size = _getSize();
					return QSize( size, size );
				}

				int SelectionView::SelectionStyleItemDelegate::_getSize() const
				{
					const QPixmap & removePixmap = Style::IconConst::get().CLOSE_PIXMAP;
					return removePixmap.height() + 2;
				}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
