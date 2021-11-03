#include "molecule_scene_view.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "action/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/widget/scene/scene_item_selection_model.hpp"
#include "ui/widget_factory.hpp"
#include "util/string.hpp"
#include "util/ui.hpp"
#include <QScrollBar>

namespace VTX::View::UI::Widget
{
	MoleculeSceneView::MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Molecule>( p_model ), SceneItemWidget( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	}

	MoleculeSceneView ::~MoleculeSceneView() { _clearLoadedItems(); }

	void MoleculeSceneView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::MOLECULE_VISIBILITY )
		{
			_refreshItemVisibility( _getMoleculeTreeWidgetItem(), _model->isVisible() );
		}
		else if ( p_event->name == Event::Model::CHAIN_VISIBILITY )
		{
			const Event::VTXEventValue<uint> * const castedEventData
				= dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
			const uint			 index = castedEventData->value;
			const Model::Chain & chain = *_model->getChain( index );

			// Check if items are visible before refresh it. If not, the will be update when they will appear
			if ( _isMoleculeExpanded() )
				_refreshItemVisibility( _getTreeWidgetItem( chain ), chain.isVisible() );
		}
		else if ( p_event->name == Event::Model::RESIDUE_VISIBILITY )

		{
			const Event::VTXEventValue<uint> * const castedEventData
				= dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
			const uint			   index   = castedEventData->value;
			const Model::Residue & residue = *_model->getResidue( index );

			// Check if items are visible before refresh it. If not, the will be update when they will appear
			if ( _isChainExpanded( *residue.getChainPtr() ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( residue ), residue.isVisible() );
			}
		}
		else if ( p_event->name == Event::Model::ATOM_VISIBILITY )
		{
			const Event::VTXEventValue<uint> * const castedEventData
				= dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
			const Model::Atom & atom = *_model->getAtom( castedEventData->value );

			if ( _isResidueExpanded( *atom.getResiduePtr() ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( atom ), atom.isVisible() );
			}
		}
		else if ( p_event->name == Event::Model::DATA_CHANGE )
		{
			_clearLoadedItems();
			_updateMoleculeStructure();
		}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			_getMoleculeTreeWidgetItem()->setText( 0, QString::fromStdString( _model->getDisplayName() ) );
		}
	}

	void MoleculeSceneView::receiveEvent( const Event::VTXEvent & p_event )
	{
		SceneItemWidget::receiveEvent( p_event );

		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER ::SYMBOL_DISPLAY_MODE ) != castedEvent.ref.end() )
			{
				_refreshSymbolDisplay( VTX_SETTING().getSymbolDisplayMode() );
			}
		}
	}

	QTreeWidgetItem * MoleculeSceneView::getLastVisibleItem()
	{
		QTreeWidgetItem * lastItemVisible = _getMoleculeTreeWidgetItem();

		while ( lastItemVisible->childCount() > 0 && lastItemVisible->isExpanded() )
			lastItemVisible = lastItemVisible->child( lastItemVisible->childCount() - 1 );

		return lastItemVisible;
	}

	void MoleculeSceneView::_setupUi( const QString & p_name ) { SceneItemWidget::_setupUi( p_name ); }
	void MoleculeSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &MoleculeSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &MoleculeSceneView::_onItemDoubleClicked );
	}

	void MoleculeSceneView::mouseMoveEvent( QMouseEvent * p_event )
	{
		setSelectionMode( QAbstractItemView::ContiguousSelection );
		SceneItemWidget::mouseMoveEvent( p_event );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
	}

	void MoleculeSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			if ( p_item == _getMoleculeTreeWidgetItem() )
			{
				std::string itemTxt = p_item->text( 0 ).toStdString();
				if ( itemTxt != _model->getDisplayName() )
				{
					_reformatMoleculeName( itemTxt );

					if ( itemTxt.empty() )
					{
						itemTxt = _model->getDisplayName();
					}
					else if ( itemTxt != _model->getDisplayName() )
					{
						VTX_ACTION( new Action::Molecule::Rename( *_model, itemTxt ) );
					}

					const bool oldSignalState = blockSignals( true );
					p_item->setText( 0, QString::fromStdString( itemTxt ) );
					blockSignals( oldSignalState );

					return;
				}
			}

			_doEnableStateChangeAction( p_item );
		}
	}
	void MoleculeSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		const Model::ID &  modelId	   = _getModelIDFromItem( *p_item );
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
	void MoleculeSceneView::_onItemExpanded( QTreeWidgetItem * const p_item )
	{
		const Model::ID &  modelId	   = _getModelIDFromItem( *p_item );
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

		_refreshSelection( Selection::SelectionManager::get().getSelectionModel() );
		SceneItemWidget::_onItemExpanded( p_item );
	}
	void MoleculeSceneView::_onItemCollapsed( QTreeWidgetItem * const p_item )
	{
		_collapseItem( *p_item );
		SceneItemWidget::_onItemCollapsed( p_item );
	}

	void MoleculeSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const Model::ID &  modelId		 = _getModelIDFromItem( *targetedItem );
		const ID::VTX_ID & modelTypeId	 = MVC::MvcManager::get().getModelTypeID( modelId );
		const QPoint	   globalClicPos = mapToGlobal( p_clicPos );

		Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
			if ( selection.isMoleculeFullySelected( molecule ) )
			{
				VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
					= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
						VTX::UI::ContextualMenu::Menu::Selection );
				selectionContextualMenu->setFocusedTarget( &molecule );
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
			}
			else
			{
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Molecule, &molecule, globalClicPos );
			}
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & chain = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
			if ( selection.isChainFullySelected( chain ) )
			{
				VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
					= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
						VTX::UI::ContextualMenu::Menu::Selection );
				selectionContextualMenu->setFocusedTarget( &chain );
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
			}
			else
			{
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Chain, &chain, globalClicPos );
			}
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & residue = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
			if ( selection.isResidueFullySelected( residue ) )
			{
				VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
					= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
						VTX::UI::ContextualMenu::Menu::Selection );
				selectionContextualMenu->setFocusedTarget( &residue );
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
			}
			else
			{
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Residue, &residue, globalClicPos );
			}
		}
		else if ( modelTypeId == ID::Model::MODEL_ATOM )
		{
			Model::Atom & atom = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
			if ( selection.isAtomSelected( atom ) )
			{
				VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
					= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
						VTX::UI::ContextualMenu::Menu::Selection );
				selectionContextualMenu->setFocusedTarget( &atom );
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
			}
			else
			{
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Atom, &atom, globalClicPos );
			}
		}
	}

	void MoleculeSceneView::_createTopLevelObject()
	{
		collapseItem( _getMoleculeTreeWidgetItem() );

		clear();
		_clearLoadedItems();

		SceneItemWidget::_createTopLevelObject();

		const QTreeWidgetItem::ChildIndicatorPolicy childIndicatorPolicy
			= _model->getChainCount() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
										  : QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		topLevelItem( 0 )->setChildIndicatorPolicy( childIndicatorPolicy );
	}

	void MoleculeSceneView::_updateMoleculeStructure()
	{
		QTreeWidgetItem * const item = _getMoleculeTreeWidgetItem();

		if ( !item->isExpanded() )
			return;

		uint currentChainIndex = 0;
		int	 currentChildIndex = 0;

		while ( currentChildIndex < item->childCount() )
		{
			QTreeWidgetItem * const currentChainItem = item->child( currentChildIndex );

			if ( currentChainIndex >= _model->getChainCount() )
			{
				delete ( item->takeChild( item->childCount() - 1 ) );
				continue;
			}

			const Model::ID &		   chainItemID = _getModelIDFromItem( *currentChainItem );
			const Model::Chain * const chain	   = _model->getChain( currentChainIndex );

			if ( chain == nullptr )
			{
				currentChainIndex++;
			}
			else if ( chainItemID != chain->getId() )
			{
				// Chain index never changes
				// if ( currentChildIndex == 0 )
				//{
				//	delete ( item->takeChild( currentChildIndex ) );
				//}
				// else
				{
					currentChainItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentChainIndex++;
				currentChildIndex++;

				if ( currentChainItem->isExpanded() )
					_updateChainStructure( *chain, *currentChainItem );
			}
		}
	}
	void MoleculeSceneView::_updateChainStructure( const Model::Chain & p_chain, QTreeWidgetItem & p_item )
	{
		const Model::Molecule & molecule			= *p_chain.getMoleculePtr();
		uint					currentResidueIndex = 0;
		int						currentChildIndex	= 0;

		while ( currentChildIndex < p_item.childCount() )
		{
			QTreeWidgetItem * const currentResidueItem = p_item.child( currentChildIndex );

			if ( currentResidueIndex >= p_chain.getResidueCount() )
			{
				delete ( p_item.takeChild( p_item.childCount() - 1 ) );
				continue;
			}

			const Model::ID &			 residueItemID = _getModelIDFromItem( *currentResidueItem );
			const Model::Residue * const residue
				= molecule.getResidue( p_chain.getIndexFirstResidue() + currentResidueIndex );

			if ( residue == nullptr )
			{
				currentResidueIndex++;
			}
			else if ( residueItemID != residue->getId() )
			{
				if ( currentChildIndex == 0 )
				{
					delete ( p_item.takeChild( currentChildIndex ) );
				}
				else
				{
					currentResidueItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentResidueIndex++;
				currentChildIndex++;

				if ( currentResidueItem->isExpanded() )
					_updateResidueStructure( *residue, *currentResidueItem );
			}
		}
	}
	void MoleculeSceneView::_updateResidueStructure( const Model::Residue & p_residue, QTreeWidgetItem & p_item )
	{
		Model::Molecule & molecule			= *p_residue.getMoleculePtr();
		uint			  currentAtomIndex	= 0;
		int				  currentChildIndex = 0;

		while ( currentChildIndex < p_item.childCount() )
		{
			QTreeWidgetItem * const currentAtomItem = p_item.child( currentChildIndex );

			// Delete all items out of range
			if ( currentAtomIndex >= p_residue.getAtomCount() )
			{
				delete ( p_item.takeChild( p_item.childCount() - 1 ) );
				continue;
			}

			const Model::ID &		  atomItemID = _getModelIDFromItem( *currentAtomItem );
			const Model::Atom * const atom		 = molecule.getAtom( p_residue.getIndexFirstAtom() + currentAtomIndex );

			if ( atom == nullptr )
			{
				currentAtomIndex++;
			}
			else if ( atomItemID != atom->getId() )
			{
				if ( currentChildIndex == 0 )
				{
					delete ( p_item.takeChild( currentChildIndex ) );
				}
				else
				{
					currentAtomItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentAtomIndex++;
				currentChildIndex++;
			}
		}
	}

	void MoleculeSceneView::_clearLoadedItems()
	{
		for ( std::pair<Model::ID, QList<QTreeWidgetItem *> *> pair : _mapLoadedItems )
		{
			QList<QTreeWidgetItem *> * const listItems = pair.second;
			while ( listItems->size() > 0 )
			{
				delete listItems->takeLast();
			}

			delete ( listItems );
		}

		_mapLoadedItems.clear();
	}

	void MoleculeSceneView::_expandAll( QTreeWidgetItem * const p_from )
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( _getModelIDFromItem( *p_from ) );

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
	void MoleculeSceneView::_expandMolecule( QTreeWidgetItem * const p_moleculeItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID &		   id = _getModelIDFromItem( *p_moleculeItem );
		QList<QTreeWidgetItem *> * itemsPtr;

		if ( p_moleculeItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_moleculeItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( id ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ id ];
			p_moleculeItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( id );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const uint chainCount = _model->getChainCount();
			nullItems.reserve( chainCount );
			items.reserve( chainCount );

			for ( const Model::Chain * const chainPtr : _model->getChains() )
			{
				QTreeWidgetItem * const chainView = new QTreeWidgetItem();

				if ( chainPtr == nullptr )
				{
					nullItems.emplace_back( chainView );
				}
				else
				{
					_applyChainDataOnItem( *chainPtr, *chainView );
				}

				items.append( chainView );
			}

			p_moleculeItem->addChildren( items );

			// setHidden Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		QTreeWidgetItem * const lastItemExpanded = *itemsPtr->rbegin();

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	 chainID = _getModelIDFromItem( *item );
			const Model::Chain & chain	 = MVC::MvcManager::get().getModel<Model::Chain>( chainID );

			const Qt::CheckState newCheckState = Util::UI::getCheckState( chain.isVisible() );
			item->setCheckState( 0, newCheckState );

			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandChain( item, p_forceExpandChildren );
			}
		}

		delete itemsPtr;

		_enableSignals( true );
	}
	void MoleculeSceneView::_expandChain( QTreeWidgetItem * const p_chainItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID &		   chainId = _getModelIDFromItem( *p_chainItem );
		QList<QTreeWidgetItem *> * itemsPtr;

		p_chainItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( p_chainItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_chainItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( chainId ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ chainId ];
			p_chainItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( chainId );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const Model::Chain & chain		  = MVC::MvcManager::get().getModel<Model::Chain>( chainId );
			const uint			 residueCount = chain.getResidueCount();
			nullItems.reserve( residueCount );
			items.reserve( residueCount );

			for ( uint i = chain.getIndexFirstResidue(); i < chain.getIndexFirstResidue() + residueCount; i++ )
			{
				const Model::Residue * const residuePtr	 = _model->getResidue( i );
				QTreeWidgetItem * const		 residueView = new QTreeWidgetItem();

				if ( residuePtr == nullptr )
				{
					nullItems.emplace_back( residueView );
				}
				else
				{
					_applyResidueDataOnItem( *residuePtr, *residueView );
				}

				items.append( residueView );
			}

			p_chainItem->addChildren( items );

			// Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		QTreeWidgetItem * const lastItemExpanded = *itemsPtr->rbegin();

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	   residueID = _getModelIDFromItem( *item );
			const Model::Residue & residue	 = MVC::MvcManager::get().getModel<Model::Residue>( residueID );

			const Qt::CheckState newCheckState = Util::UI::getCheckState( residue.isVisible() );
			item->setCheckState( 0, newCheckState );

			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandResidue( item, p_forceExpandChildren );
			}
		}

		delete ( itemsPtr );

		_enableSignals( true );
	}
	void MoleculeSceneView::_expandResidue( QTreeWidgetItem * const p_residueItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		const Model::ID &		   residueId = _getModelIDFromItem( *p_residueItem );
		QList<QTreeWidgetItem *> * itemsPtr;

		p_residueItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( p_residueItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_residueItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( residueId ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ residueId ];
			p_residueItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( residueId );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const Model::Residue & residue	 = MVC::MvcManager::get().getModel<Model::Residue>( residueId );
			const uint			   atomCount = residue.getAtomCount();
			nullItems.reserve( atomCount );
			items.reserve( atomCount );

			for ( uint i = residue.getIndexFirstAtom(); i < residue.getIndexFirstAtom() + atomCount; i++ )
			{
				const Model::Atom * const atomPtr  = _model->getAtom( i );
				QTreeWidgetItem * const	  atomView = new QTreeWidgetItem();

				if ( atomPtr == nullptr )
				{
					nullItems.emplace_back( atomView );
				}
				else
				{
					_applyAtomDataOnItem( *atomPtr, *atomView );
				}

				items.append( atomView );
			}

			p_residueItem->addChildren( items );

			// Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		QTreeWidgetItem * const lastItemExpanded = *itemsPtr->rbegin();

		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	atomID = _getModelIDFromItem( *item );
			const Model::Atom & atom   = MVC::MvcManager::get().getModel<Model::Atom>( atomID );

			const Qt::CheckState newCheckState = Util::UI::getCheckState( atom.isVisible() );
			item->setCheckState( 0, newCheckState );
		}

		_enableSignals( true );
	}

	void MoleculeSceneView::_collapseItem( QTreeWidgetItem & p_item )
	{
		_enableSignals( false );

		p_item.setData( 0, EXPAND_STATE_ROLE, false );

		const Model::ID &				 id	   = _getModelIDFromItem( p_item );
		QList<QTreeWidgetItem *> * const items = new QList( p_item.takeChildren() );
		_mapLoadedItems.emplace( id, items );

		_enableSignals( true );
	}

	void MoleculeSceneView::_fillListWithItemChildren( const QTreeWidgetItem &	  p_parent,
													   QList<QTreeWidgetItem *> & p_list ) const
	{
		const uint childCount = p_parent.childCount();
		p_list.reserve( childCount );

		for ( uint i = 0; i < childCount; i++ )
			p_list.append( p_parent.child( i ) );
	}
	void MoleculeSceneView::_applyMoleculeDataOnItem( const Model::Molecule & p_molecule,
													  QTreeWidgetItem &		  p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::Model::ID>( p_molecule.getId() ) );
		p_item.setText( 0, QString::fromStdString( p_molecule.getDisplayName() ) );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_molecule.getTypeId() ) );

		const QTreeWidgetItem::ChildIndicatorPolicy childIndicatorPolicy
			= p_molecule.getChainCount() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
											 : QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		p_item.setChildIndicatorPolicy( childIndicatorPolicy );
	}
	void MoleculeSceneView::_applyChainDataOnItem( const Model::Chain & p_chain, QTreeWidgetItem & p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_chain.getId() ) );
		p_item.setText( 0, QString::fromStdString( p_chain.getDefaultName() ) );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_chain.getTypeId() ) );

		// Always show indicator, if cbain has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void MoleculeSceneView::_applyResidueDataOnItem( const Model::Residue & p_residue, QTreeWidgetItem & p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_residue.getId() ) );
		_applyResidueNameOnItem( p_residue, p_item, VTX_SETTING().getSymbolDisplayMode() );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_residue.getTypeId() ) );

		// Always show indicator, if residue has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void MoleculeSceneView::_applyAtomDataOnItem( const Model::Atom & p_atom, QTreeWidgetItem & p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_atom.getId() ) );
		p_item.setText( 0,
						QString::fromStdString( p_atom.getSymbolStr() + " " + std::to_string( p_atom.getIndex() ) ) );
		p_item.setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_atom.getTypeId() ) );
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator );
	}

	void MoleculeSceneView::_applyResidueNameOnItem( const Model::Residue &				p_residue,
													 QTreeWidgetItem &					p_item,
													 const Style::SYMBOL_DISPLAY_MODE & p_symbolDisplayMode ) const
	{
		const std::string * text;
		switch ( p_symbolDisplayMode )
		{
		case Style::SYMBOL_DISPLAY_MODE::SHORT: text = &p_residue.getSymbolStr(); break;
		case Style::SYMBOL_DISPLAY_MODE::LONG: text = &p_residue.getSymbolName(); break;
		default:
			VTX_WARNING( "Symbol style " + std::to_string( int( p_symbolDisplayMode ) )
						 + " not managed in MoleculeSceneView::_applyResidueNameOnItem." );

			text = &p_residue.getSymbolStr();
			break;
		}

		p_item.setText( 0,
						QString::fromStdString( *text + " " + std::to_string( p_residue.getIndexInOriginalChain() ) ) );
	}

	void MoleculeSceneView::_doEnableStateChangeAction( const QTreeWidgetItem * const p_item ) const
	{
		const Model::ID &  modelId		= _getModelIDFromItem( *p_item );
		const ID::VTX_ID & modelTypeId	= MVC::MvcManager::get().getModelTypeID( modelId );
		const bool		   modelEnabled = p_item->checkState( 0 ) == Qt::CheckState::Checked ? true : false;

		const Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();

		const Action::Visible::ChangeVisibility::VISIBILITY_MODE visibilityMode
			= modelEnabled ? Action::Visible::ChangeVisibility::VISIBILITY_MODE::SHOW
						   : Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE;

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );

			if ( selection.isMoleculeFullySelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Molecule::ChangeVisibility( model, visibilityMode ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );

			if ( selection.isChainFullySelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Chain::ChangeVisibility( model, visibilityMode ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );

			if ( selection.isResidueFullySelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Residue::ChangeVisibility( model, visibilityMode ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_ATOM )
		{
			Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );

			if ( selection.isAtomSelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Atom::ChangeVisibility( model, visibilityMode ) );
		}
	}
	void MoleculeSceneView::_reformatMoleculeName( std::string & p_moleculeName ) const
	{
		// Trim
		Util::String::trim( p_moleculeName );

		// Forbid line feed
		const size_t firstLinefeedIndex = p_moleculeName.find_first_of( '\n' );
		if ( firstLinefeedIndex != std::string::npos )
			p_moleculeName.erase( firstLinefeedIndex );

		// Clamp size
		if ( p_moleculeName.size() > Style::MOLECULE_NAME_MAXIMUM_SIZE )
			p_moleculeName = p_moleculeName.substr( 0, Style::MOLECULE_NAME_MAXIMUM_SIZE );
	}

	void MoleculeSceneView::_fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection )
	{
		const Model::Selection::MapMoleculeIds &			   items		  = p_selection.getMoleculesMap();
		const Model::Selection::MapMoleculeIds::const_iterator itMoleculeItem = items.find( _model->getId() );

		if ( itMoleculeItem != items.end() )
		{
			QTreeWidgetItem * const moleculeItem = _getMoleculeTreeWidgetItem();
			p_itemSelection.append( QItemSelectionRange( indexFromItem( moleculeItem ) ) );

			if ( moleculeItem->isExpanded() )
			{
				const Model::Chain * topChain			  = nullptr;
				const Model::Chain * previousChain		  = nullptr;
				QModelIndex			 topChainItemIndex	  = QModelIndex();
				QModelIndex			 bottomChainItemIndex = QModelIndex();

				for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : itMoleculeItem->second )
				{
					const Model::Chain & chain = *_model->getChain( pairChain.first );

					QTreeWidgetItem * const chainItem = moleculeItem->child( chain.getIndex() );

					if ( topChainItemIndex.isValid() )
					{
						// if not contiguous, add new range
						if ( chain.getIndex() != uint( previousChain->getIndex() + 1 ) )
						{
							p_itemSelection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
							topChainItemIndex = indexFromItem( chainItem );
							topChain		  = &chain;
						}

						bottomChainItemIndex = indexFromItem( chainItem );
					}
					else
					{
						topChainItemIndex	 = indexFromItem( chainItem );
						topChain			 = &chain;
						bottomChainItemIndex = indexFromItem( chainItem );
					}

					previousChain = &chain;

					if ( !chainItem->isExpanded() )
						continue;

					const Model::Residue * topResidue			  = nullptr;
					const Model::Residue * previousResidue		  = nullptr;
					QModelIndex			   topResidueItemIndex	  = QModelIndex();
					QModelIndex			   bottomResidueItemIndex = QModelIndex();
					for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
					{
						const Model::Residue &	residue = *_model->getResidue( pairResidue.first );
						QTreeWidgetItem * const residueItem
							= chainItem->child( residue.getIndex() - chain.getIndexFirstResidue() );

						if ( topResidueItemIndex.isValid() )
						{
							// if not contiguous, add new range
							if ( residue.getIndex() != uint( previousResidue->getIndex() + 1 ) )
							{
								p_itemSelection.append(
									QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
								topResidueItemIndex = indexFromItem( residueItem );
								topResidue			= &residue;
							}

							bottomResidueItemIndex = indexFromItem( residueItem );
						}
						else
						{
							topResidueItemIndex	   = indexFromItem( residueItem );
							topResidue			   = &residue;
							bottomResidueItemIndex = indexFromItem( residueItem );
						}

						previousResidue = &residue;

						if ( !residueItem->isExpanded() )
							continue;

						const Model::Atom * topAtom				= nullptr;
						const Model::Atom * previousAtom		= nullptr;
						QModelIndex			topAtomItemIndex	= QModelIndex();
						QModelIndex			bottomAtomItemIndex = QModelIndex();
						for ( const uint & atomId : pairResidue.second )
						{
							const Model::Atom &		atom = *_model->getAtom( atomId );
							QTreeWidgetItem * const atomItem
								= residueItem->child( atomId - residue.getIndexFirstAtom() );

							if ( topAtomItemIndex.isValid() )
							{
								// if not contiguous, add new range
								if ( atom.getIndex() != uint( previousAtom->getIndex() + 1 ) )
								{
									p_itemSelection.append(
										QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
									topAtom			 = &atom;
									topAtomItemIndex = indexFromItem( atomItem );
								}

								bottomAtomItemIndex = indexFromItem( atomItem );
							}
							else
							{
								topAtomItemIndex	= indexFromItem( atomItem );
								topAtom				= &atom;
								bottomAtomItemIndex = indexFromItem( atomItem );
							}

							previousAtom = &atom;
						}
						if ( topAtomItemIndex.isValid() )
						{
							p_itemSelection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
						}
					}

					if ( topResidueItemIndex.isValid() )
					{
						p_itemSelection.append( QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
					}
				}

				if ( topChainItemIndex.isValid() )
				{
					p_itemSelection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
				}
			}
		}
	}

	void MoleculeSceneView::_refreshSymbolDisplay( const Style::SYMBOL_DISPLAY_MODE & p_displayMode )
	{
		_refreshSymbolDisplayRecursive( _getMoleculeTreeWidgetItem(), p_displayMode );
		_clearLoadedItems();
	}
	void MoleculeSceneView::_refreshSymbolDisplayRecursive( QTreeWidgetItem * const			   p_item,
															const Style::SYMBOL_DISPLAY_MODE & p_displayMode )
	{
		const Model::ID &  modelId	   = _getModelIDFromItem( *p_item );
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );

		if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			for ( int i = 0; i < p_item->childCount(); i++ )
			{
				QTreeWidgetItem * const child	= p_item->child( i );
				const Model::ID &		modelId = _getModelIDFromItem( *child );
				const Model::Residue &	residue = MVC::MvcManager::get().getModel<Model::Residue>( modelId );

				_applyResidueNameOnItem( residue, *child, p_displayMode );
			}
		}
		else
		{
			for ( int i = 0; i < p_item->childCount(); i++ )
			{
				_refreshSymbolDisplayRecursive( p_item->child( i ), p_displayMode );
			}
		}
	}

	QTreeWidgetItem * const MoleculeSceneView::_getMoleculeTreeWidgetItem() const { return topLevelItem( 0 ); }
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem( const Model::ID & p_id ) const
	{
		QTreeWidgetItem * res;

		const ID::VTX_ID & typeID = MVC::MvcManager::get().getModelTypeID( p_id );

		if ( typeID == ID::Model::MODEL_MOLECULE )
			res = _getMoleculeTreeWidgetItem();
		else if ( typeID == ID::Model::MODEL_CHAIN )
			res = _getTreeWidgetItem( MVC::MvcManager::get().getModel<Model::Chain>( p_id ) );
		else if ( typeID == ID::Model::MODEL_RESIDUE )
			res = _getTreeWidgetItem( MVC::MvcManager::get().getModel<Model::Residue>( p_id ) );
		else if ( typeID == ID::Model::MODEL_ATOM )
			res = _getTreeWidgetItem( MVC::MvcManager::get().getModel<Model::Atom>( p_id ) );
		else
			res = nullptr;

		return res;
	}

	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem( const Model::Chain & p_chain ) const
	{
		return topLevelItem( 0 )->child( p_chain.getIndex() );
	}
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem( const Model::Residue & p_residue ) const
	{
		const Model::Chain * const chain	  = p_residue.getChainPtr();
		const uint				   childIndex = p_residue.getIndex() - chain->getIndexFirstResidue();

		return _getTreeWidgetItem( *chain )->child( childIndex );
	}
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem( const Model::Atom & p_atom ) const
	{
		const Model::Residue * const residue	= p_atom.getResiduePtr();
		const uint					 childIndex = p_atom.getIndex() - residue->getIndexFirstAtom();

		return _getTreeWidgetItem( *residue )->child( childIndex );
	}

	bool MoleculeSceneView::_isMoleculeExpanded() const { return _getMoleculeTreeWidgetItem()->childCount() > 0; }
	bool MoleculeSceneView::_isChainExpanded( const Model::Chain & p_chain ) const
	{
		return _isMoleculeExpanded() && _getTreeWidgetItem( p_chain )->childCount() > 0;
	}
	bool MoleculeSceneView::_isResidueExpanded( const Model::Residue & p_residue ) const
	{
		const Model::Chain * const chain = p_residue.getChainPtr();
		return _isChainExpanded( *chain ) && _getTreeWidgetItem( p_residue )->childCount() > 0;
	}

	QMimeData * MoleculeSceneView::_getDataForDrag() { return VTX::UI::MimeType::generateMoleculeData( *_model ); }
	bool		MoleculeSceneView::_canDragObjectAtPos( const QPoint & p_position )
	{
		// Can only drag from the molecule
		return itemAt( p_position ) == _getMoleculeTreeWidgetItem();
	}

	void MoleculeSceneView::updatePosInSceneHierarchy( const int p_position )
	{
		SceneItemWidget::updatePosInSceneHierarchy( p_position );
		_model->getConfiguration().sceneIndex = p_position;
	}

	bool MoleculeSceneView::_itemCanBeRenamed( const QTreeWidgetItem * p_item )
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		return p_item == _getMoleculeTreeWidgetItem() && selection.isMoleculeFullySelected( *_model );
	}

} // namespace VTX::View::UI::Widget
