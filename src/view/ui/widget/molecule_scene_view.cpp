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
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget/scene/molecule_selection_model.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::View::UI::Widget
{
	MoleculeSceneView::MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Molecule>( p_model ), SceneItemWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	MoleculeSceneView ::~MoleculeSceneView() { _clearLoadedItems(); }

	void MoleculeSceneView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::MOLECULE_VISIBILITY )
		{
			_refreshItemVisibility( _getMoleculeTreeWidgetItem(), *_model );
		}
		else if ( p_event->name == Event::Model::CHAIN_VISIBILITY )
		{
			const Event::VTXEventValue<uint> * const castedEventData
				= dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
			const uint			 index = castedEventData->value;
			const Model::Chain & chain = *_model->getChain( index );

			// Check if items are visible before refresh it. If not, the will be update when they will appear
			if ( _isMoleculeExpanded() )
				_refreshItemVisibility( _getTreeWidgetItem( chain ), chain );
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
				_refreshItemVisibility( _getTreeWidgetItem( residue ), residue );
			}
		}
		else if ( p_event->name == Event::Model::ATOM_VISIBILITY )
		{
			const Event::VTXEventValue<uint> * const castedEventData
				= dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
			const Model::Atom & atom = *_model->getAtom( castedEventData->value );

			if ( _isResidueExpanded( *atom.getResiduePtr() ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( atom ), atom );
			}
		}
		else if ( p_event->name == Event::Model::DATA_CHANGE )
		{
			_rebuildTree();
		}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			_getMoleculeTreeWidgetItem()->setText( 0, QString::fromStdString( _model->getDisplayName() ) );
		}
	}

	void MoleculeSceneView::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			_refreshSelection( *castedEvent.ptr );
		}
	}

	void MoleculeSceneView::keyPressEvent( QKeyEvent * p_event )
	{
		// Desactivate expand all (*) shortcut
		if ( p_event->key() == Qt::Key::Key_Asterisk )
		{
			return;
			// Reimplement expand all action to prevent useless multiple call to refreshSelection
			//_expandAll( currentItem() );
		}
		else if ( p_event->key() == Qt::Key::Key_F2 )
		{
			const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

			// Override rename key binding because multiple selection with molecule make it fail
			if ( currentItem() == _getMoleculeTreeWidgetItem() && selection.isMoleculeFullySelected( *_model )
				 && selection.getItems().size() == 1 )
			{
				openRenameEditor();
			}
		}
		else
		{
			SceneItemWidget::keyPressEvent( p_event );
		}
	}

	void MoleculeSceneView::_setupUi( const QString & p_name )
	{
		SceneItemWidget::_setupUi( p_name );
		setSelectionModel( new VTX::UI::Widget ::Scene::MoleculeSelectionModel( _model, model(), this ) );
		setExpandsOnDoubleClick( false );

		setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		setEditTriggers( EditTrigger::SelectedClicked );

		_rebuildTree();
	}
	void MoleculeSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &MoleculeSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &MoleculeSceneView::_onItemDoubleClicked );
		connect( this, &QTreeWidget::itemExpanded, this, &MoleculeSceneView::_onItemExpanded );
		connect( this, &QTreeWidget::itemCollapsed, this, &MoleculeSceneView::_onItemCollapsed );

		connect( this, &QTreeWidget::customContextMenuRequested, this, &MoleculeSceneView::_onCustomContextMenuCalled );
	}
	void MoleculeSceneView::localize() { SceneItemWidget::localize(); }

	void MoleculeSceneView::mouseMoveEvent( QMouseEvent * p_event )
	{
		setSelectionMode( QAbstractItemView::ContiguousSelection );
		SceneItemWidget::mouseMoveEvent( p_event );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
	}

	void MoleculeSceneView::_onItemChanged( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		if ( p_column == 0 )
		{
			if ( p_item == _getMoleculeTreeWidgetItem() )
			{
				const std::string itemTxt = p_item->text( 0 ).toStdString();
				if ( itemTxt != _model->getDisplayName() )
				{
					VTX_ACTION( new Action::Molecule::Rename( *_model, itemTxt ) );
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
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
			}
			else
			{
				VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Atom, &atom, globalClicPos );
			}
		}
	}

	void MoleculeSceneView::_rebuildTree()
	{
		collapseItem( _getMoleculeTreeWidgetItem() );

		clear();
		_clearLoadedItems();

		QTreeWidgetItem * const moleculeView = new QTreeWidgetItem();
		moleculeView->setFlags( moleculeView->flags() | Qt::ItemFlag::ItemIsEditable );

		_applyMoleculeDataOnItem( *_model, *moleculeView );
		_refreshItemVisibility( moleculeView, *_model );

		addTopLevelItem( moleculeView );
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

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	 chainID = _getModelIDFromItem( *item );
			const Model::Chain & chain	 = MVC::MvcManager::get().getModel<Model::Chain>( chainID );

			const Qt::CheckState newCheckState = _getCheckState( chain.isVisible() );
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

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	   residueID = _getModelIDFromItem( *item );
			const Model::Residue & residue	 = MVC::MvcManager::get().getModel<Model::Residue>( residueID );

			const Qt::CheckState newCheckState = _getCheckState( residue.isVisible() );
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

		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const Model::ID &	atomID = _getModelIDFromItem( *item );
			const Model::Atom & atom   = MVC::MvcManager::get().getModel<Model::Atom>( atomID );

			const Qt::CheckState newCheckState = _getCheckState( atom.isVisible() );
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
		p_item.setText( 0,
						QString::fromStdString( p_residue.getSymbolStr() + " "
												+ std::to_string( p_residue.getIndexInOriginalChain() ) ) );
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

			if ( selection.isMoleculeSelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Molecule::ChangeVisibility( model, visibilityMode ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );

			if ( selection.isChainSelected( model ) )
				VTX_ACTION( new Action::Selection::ChangeVisibility( selection, model, modelTypeId, visibilityMode ) );
			else
				VTX_ACTION( new Action::Chain::ChangeVisibility( model, visibilityMode ) );
		}
		else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );

			if ( selection.isResidueSelected( model ) )
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

	void MoleculeSceneView::_refreshItemVisibility( QTreeWidgetItem * const		 p_itemWidget,
													const Generic::BaseVisible & p_baseVisible )
	{
		_enableSignals( false );

		const Qt::CheckState newCheckState = _getCheckState( p_baseVisible.isVisible() );
		if ( p_itemWidget->checkState( 0 ) != newCheckState )
			p_itemWidget->setCheckState( 0, newCheckState );

		_enableSignals( true );
	}
	void MoleculeSceneView::_refreshSelection( const Model::Selection & p_selection )
	{
		const Model::Selection::MapMoleculeIds &			   items = p_selection.getItems();
		const Model::Selection::MapMoleculeIds::const_iterator itMoleculeItem
			= p_selection.getItems().find( _model->getId() );

		_enableSignals( false );
		QItemSelection selection = QItemSelection();

		if ( itMoleculeItem != p_selection.getItems().end() )
		{
			QTreeWidgetItem * const moleculeItem = _getMoleculeTreeWidgetItem();
			selection.append( QItemSelectionRange( indexFromItem( moleculeItem ) ) );

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
							selection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
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
								selection.append( QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
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
									selection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
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
							selection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
						}
					}

					if ( topResidueItemIndex.isValid() )
					{
						selection.append( QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
					}
				}

				if ( topChainItemIndex.isValid() )
				{
					selection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
				}
			}
		}

		VTX::UI::Widget::Scene::MoleculeSelectionModel * const selectModel
			= dynamic_cast<VTX::UI::Widget::Scene::MoleculeSelectionModel * const>( selectionModel() );

		selectModel->refreshSelection( selection );

		_enableSignals( true );
	}

	void MoleculeSceneView::_enableSignals( const bool p_enable )
	{
		if ( p_enable )
			_enableSignalCounter--;
		else
			_enableSignalCounter++;

		const bool isEnable = _enableSignalCounter == 0;
		blockSignals( !isEnable );
		setUpdatesEnabled( isEnable );
	}

	Model::ID MoleculeSceneView::_getModelIDFromItem( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & dataID = p_item.data( 0, MODEL_ID_ROLE );
		return dataID.value<VTX::Model::ID>();
	}
	bool MoleculeSceneView::_getItemExpandState( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & expandState = p_item.data( 0, EXPAND_STATE_ROLE );
		return expandState.isValid() && expandState.value<bool>();
	}

	QTreeWidgetItem * const MoleculeSceneView::_getMoleculeTreeWidgetItem() const { return topLevelItem( 0 ); }
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
		return _isChainExpanded( *chain ) && _getTreeWidgetItem( *chain )->childCount() > 0;
	}

	QMimeData * MoleculeSceneView::_getDataForDrag() { return VTX::UI::MimeType::generateMoleculeData( *_model ); }
	bool		MoleculeSceneView::_canDragObjectAtPos( const QPoint & p_position )
	{
		// Can only drag from the molecule
		return itemAt( p_position ) == _getMoleculeTreeWidgetItem();
	}

	void MoleculeSceneView::openRenameEditor() { editItem( _getMoleculeTreeWidgetItem() ); }

} // namespace VTX::View::UI::Widget
