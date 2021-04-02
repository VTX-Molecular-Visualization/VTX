#include "selection_view.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QHeaderView>

namespace VTX::View::UI::Widget
{
	SelectionView ::~SelectionView() { _mapLoadedItems.clear(); }

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

	void SelectionView::_setupSlots()
	{
		connect( this, &QTreeWidget::itemClicked, this, &SelectionView::_onItemClicked );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &SelectionView::_onItemDoubleClicked );
		connect( this, &QTreeWidget::itemExpanded, this, &SelectionView::_onItemExpanded );
		connect( this, &QTreeWidget::itemCollapsed, this, &SelectionView::_onItemCollapsed );
	}

	void SelectionView::_refreshView()
	{
		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();

		_enableSignals( false );

		_mapLoadedItems.clear();

		const Model::Selection::MapMoleculeIds & items = _model->getItems();
		uint									 m	   = 0;

		QList<QTreeWidgetItem *> children = invisibleRootItem()->takeChildren();

		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & pairMolecule : items )
		{
			QTreeWidgetItem * moleculeView = nullptr;
			const Model::ID & moleculeId   = pairMolecule.first;

			moleculeView = _extractItemFromList( moleculeId, children );

			if ( moleculeView == nullptr )
			{
				moleculeView = new QTreeWidgetItem();
				const Model::Molecule & molecule
					= MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
				_applyMoleculeDataOnItem( molecule, *moleculeView, pairMolecule.second );
			}

			addTopLevelItem( moleculeView );

			if ( _getItemExpandState( *moleculeView ) )
			{
				moleculeView->setExpanded( true );
				_expandMolecule( moleculeView );
			}
		}

		_clearList( children );

		_enableSignals( true );

		chrono.stop();
	}

	void SelectionView::_expandAll( QTreeWidgetItem * const p_from )
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( _getModelID( *p_from ) );

		_enableSignals( false );

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
			_expandMolecule( p_from, true );
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
			_expandChain( p_from, true );
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
			_expandResidue( p_from, true );

		_enableSignals( true );

		p_from->setExpanded( true );
	}
	void SelectionView::_expandMolecule( QTreeWidgetItem * const p_moleculeItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID &		   moleculeId = _getModelID( *p_moleculeItem );
		QList<QTreeWidgetItem *> * itemsPtr;

		p_moleculeItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( _mapLoadedItems.find( moleculeId ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ moleculeId ];
			p_moleculeItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( moleculeId );
		}
		else
		{
			itemsPtr						 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> & items = *itemsPtr;

			const Model::Selection::MapChainIds & chainIds = _model->getItems().at( moleculeId );
			const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeId );

			const uint chainCount = uint( chainIds.size() );
			items.reserve( chainCount );

			QList<QTreeWidgetItem *> children = p_moleculeItem->takeChildren();

			for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : chainIds )
			{
				const Model::Chain & chain = *molecule.getChain( pairChain.first );

				QTreeWidgetItem * chainView = _extractItemFromList( chain.getId(), children );

				if ( chainView == nullptr )
				{
					chainView = new QTreeWidgetItem();
					_applyChainDataOnItem( chain, *chainView, pairChain.second );
				}

				items.append( chainView );
			}

			_clearList( children );
			p_moleculeItem->addChildren( items );
		}

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandChain( item, p_forceExpandChildren );
			}
		}

		delete itemsPtr;

		_enableSignals( true );
	}
	void SelectionView::_expandChain( QTreeWidgetItem * const p_chainItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID &		   chainId = _getModelID( *p_chainItem );
		QList<QTreeWidgetItem *> * itemsPtr;

		p_chainItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( _mapLoadedItems.find( chainId ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ chainId ];
			p_chainItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( chainId );
		}
		else
		{
			itemsPtr						 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> & items = *itemsPtr;

			const Model::Chain & chain = MVC::MvcManager::get().getModel<Model::Chain>( chainId );

			const Model::ID &						moleculeId = chain.getMoleculePtr()->getId();
			const uint								chainIndex = chain.getIndex();
			const Model::Selection::MapResidueIds & residueIds = _model->getItems().at( moleculeId ).at( chainIndex );

			const uint residueCount = uint( residueIds.size() );
			items.reserve( residueCount );

			QList<QTreeWidgetItem *> children = p_chainItem->takeChildren();

			const Model::Molecule & molecule = *chain.getMoleculePtr();
			for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : residueIds )
			{
				const Model::Residue & residue	   = *molecule.getResidue( pairResidue.first );
				QTreeWidgetItem *	   residueView = _extractItemFromList( residue.getId(), children );

				if ( residueView == nullptr )
				{
					residueView = new QTreeWidgetItem();
					_applyResidueDataOnItem( residue, *residueView, pairResidue.second );
				}

				items.append( residueView );
			}

			_clearList( children );
			p_chainItem->addChildren( items );
		}

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandResidue( item, p_forceExpandChildren );
			}
		}

		delete ( itemsPtr );

		_enableSignals( true );
	}
	void SelectionView::_expandResidue( QTreeWidgetItem * const p_residueItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID & residueId = _getModelID( *p_residueItem );

		p_residueItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( _mapLoadedItems.find( residueId ) != _mapLoadedItems.end() )
		{
			p_residueItem->addChildren( *_mapLoadedItems[ residueId ] );
			_mapLoadedItems.erase( residueId );
		}
		else
		{
			QList<QTreeWidgetItem *> items = QList<QTreeWidgetItem *>();

			const Model::Residue &	residue	   = MVC::MvcManager::get().getModel<Model::Residue>( residueId );
			const Model::Chain &	chain	   = *residue.getChainPtr();
			const Model::Molecule & molecule   = *chain.getMoleculePtr();
			const Model::ID			moleculeId = molecule.getId();

			const uint residueIndex = residue.getIndex();
			const uint chainIndex	= chain.getIndex();

			const Model::Selection::VecAtomIds & vecAtoms
				= _model->getItems().at( moleculeId ).at( chainIndex ).at( residueIndex );

			QList<QTreeWidgetItem *> children = p_residueItem->takeChildren();

			const uint atomCount = uint( vecAtoms.size() );
			items.reserve( atomCount );

			for ( const uint & atomIndex : vecAtoms )
			{
				const Model::Atom * const atomPtr  = molecule.getAtom( atomIndex );
				QTreeWidgetItem *		  atomView = _extractItemFromList( atomPtr->getId(), children );

				if ( atomView == nullptr )
				{
					atomView = new QTreeWidgetItem();
					_applyAtomDataOnItem( *atomPtr, *atomView );
				}

				items.append( atomView );
			}

			_clearList( children );
			p_residueItem->addChildren( items );
		}

		_enableSignals( true );
	}

	void SelectionView::_collapseItem( QTreeWidgetItem & p_item )
	{
		_enableSignals( false );

		p_item.setData( 0, EXPAND_STATE_ROLE, false );

		const Model::ID &				 id	   = _getModelID( p_item );
		QList<QTreeWidgetItem *> * const items = new QList( p_item.takeChildren() );
		_mapLoadedItems.emplace( id, items );

		_enableSignals( true );
	}

	QTreeWidgetItem * SelectionView::_extractItemFromList( const Model::ID &		  p_id,
														   QList<QTreeWidgetItem *> & p_list ) const
	{
		for ( int i = 0; i < p_list.size(); i++ )
		{
			if ( _getModelID( *p_list[ i ] ) == p_id )
			{
				return p_list.takeAt( i );
			}
		}

		return nullptr;
	}
	void SelectionView::_clearList( QList<QTreeWidgetItem *> & p_list ) const
	{
		while ( p_list.size() > 0 )
		{
			delete ( p_list.takeAt( 0 ) );
		}
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
	void SelectionView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		const Model::ID &  modelId	   = _getModelID( *p_item );
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
			VTX_ACTION( new Action::Molecule::Orient( model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
			VTX_ACTION( new Action::Chain::Orient( model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
			VTX_ACTION( new Action::Residue::Orient( model ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
			VTX_ACTION( new Action::Atom::Orient( model ) );
		}
	}
	void SelectionView::_onItemExpanded( QTreeWidgetItem * const p_item )
	{
		// If expanded, height is good at minimum height, we reset it.
		setMinimumHeight( 0 );
		setMinimumWidth( sizeHintForColumn( 0 ) );

		const Model::ID &  modelId	   = _getModelID( *p_item );
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );

		if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
		{
			_expandMolecule( p_item );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
		{
			_expandChain( p_item );
		}
		else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
		{
			_expandResidue( p_item );
		}
	}
	void SelectionView::_onItemCollapsed( QTreeWidgetItem * const p_item )
	{
		// Minimum height is bad when full collapsed => we force it.
		setMinimumHeight( topLevelItem( 0 )->isExpanded() ? 0 : rowHeight( model()->index( 0, 0 ) ) );
		setMinimumWidth( sizeHintForColumn( 0 ) );

		_collapseItem( *p_item );
	}

	void SelectionView::localize() {}

	void SelectionView::_applyMoleculeDataOnItem( const Model::Molecule &				p_molecule,
												  QTreeWidgetItem &						p_item,
												  const Model::Selection::MapChainIds & p_children ) const
	{
		p_item.setData( NAME_COLUMN_INDEX, MODEL_ID_ROLE, QVariant::fromValue<VTX::Model::ID>( p_molecule.getId() ) );
		p_item.setText( NAME_COLUMN_INDEX, QString::fromStdString( p_molecule.getDefaultName() ) );
		p_item.setIcon( NAME_COLUMN_INDEX, *VTX::Style::IconConst::get().getModelSymbol( p_molecule.getTypeId() ) );

		const QTreeWidgetItem::ChildIndicatorPolicy childIndicatorPolicy
			= p_children.size() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
									: QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		p_item.setChildIndicatorPolicy( childIndicatorPolicy );
	}
	void SelectionView::_applyChainDataOnItem( const Model::Chain &					   p_chain,
											   QTreeWidgetItem &					   p_item,
											   const Model::Selection::MapResidueIds & p_children ) const
	{
		p_item.setData( NAME_COLUMN_INDEX, MODEL_ID_ROLE, QVariant::fromValue( p_chain.getId() ) );
		p_item.setText( NAME_COLUMN_INDEX, QString::fromStdString( p_chain.getDefaultName() ) );
		p_item.setIcon( NAME_COLUMN_INDEX, *VTX::Style::IconConst::get().getModelSymbol( p_chain.getTypeId() ) );

		QTreeWidgetItem::ChildIndicatorPolicy childIndicator
			= p_children.size() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
									: QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		// Always show indicator, if cbain has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void SelectionView::_applyResidueDataOnItem( const Model::Residue &				  p_residue,
												 QTreeWidgetItem &					  p_item,
												 const Model::Selection::VecAtomIds & p_children ) const
	{
		p_item.setData( NAME_COLUMN_INDEX, MODEL_ID_ROLE, QVariant::fromValue( p_residue.getId() ) );
		p_item.setText( NAME_COLUMN_INDEX,
						QString::fromStdString( p_residue.getSymbolStr() + " "
												+ std::to_string( p_residue.getIndexInOriginalChain() ) ) );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_residue.getTypeId() ) );

		// Always show indicator, if residue has no child, it is remove from the molecule
		QTreeWidgetItem::ChildIndicatorPolicy childIndicator
			= p_children.size() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
									: QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		p_item.setChildIndicatorPolicy( childIndicator );
	}
	void SelectionView::_applyAtomDataOnItem( const Model::Atom & p_atom, QTreeWidgetItem & p_item ) const
	{
		p_item.setData( NAME_COLUMN_INDEX, MODEL_ID_ROLE, QVariant::fromValue( p_atom.getId() ) );
		p_item.setText( NAME_COLUMN_INDEX,
						QString::fromStdString( p_atom.getSymbolStr() + " " + std::to_string( p_atom.getIndex() ) ) );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_atom.getTypeId() ) );
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator );
	}

	Model::ID SelectionView::_getModelID( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & dataID = p_item.data( NAME_COLUMN_INDEX, MODEL_ID_ROLE );
		return dataID.value<VTX::Model::ID>();
	}
	bool SelectionView::_getItemExpandState( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & expandState = p_item.data( 0, EXPAND_STATE_ROLE );
		return expandState.isValid() && expandState.value<bool>();
	}
	void SelectionView::_enableSignals( const bool p_enable )
	{
		if ( p_enable )
			_enableSignalCounter--;
		else
			_enableSignalCounter++;

		const bool isEnable = _enableSignalCounter == 0;
		blockSignals( !isEnable );
		setUpdatesEnabled( isEnable );
	}

	// /////////////////////  SelectionStyleItemDelegate  ///////////////////////////
	void SelectionView::SelectionStyleItemDelegate::paint( QPainter *					painter,
														   const QStyleOptionViewItem & option,
														   const QModelIndex &			index ) const
	{
		if ( option.state & QStyle::State_MouseOver )
			painter->fillRect( option.rect, option.palette.highlight() );

		const int minColumnBorder
			= ( option.rect.width() < option.rect.height() ? option.rect.width() : option.rect.height() );
		const int size = ( minColumnBorder < _getSize() ? minColumnBorder : _getSize() ) - 2;

		const QRect rect = QRect( option.rect.x() + ( ( option.rect.width() - size ) / 2 ),
								  option.rect.y() + ( ( option.rect.height() - size ) / 2 ),
								  size,
								  size );
		painter->drawPixmap( rect, Style::IconConst::get().CLOSE_PIXMAP );
	}

	QSize SelectionView::SelectionStyleItemDelegate::sizeHint( const QStyleOptionViewItem & option,
															   const QModelIndex &			index ) const
	{
		const int size = _getSize();
		return QSize( size, size );
	}

	int SelectionView::SelectionStyleItemDelegate::_getSize() const
	{
		const QPixmap & removePixmap = Style::IconConst::get().CLOSE_PIXMAP;
		return removePixmap.height() + 2;
	}

} // namespace VTX::View::UI::Widget
