#include "scene_item_widget.hpp"
#include "generic/base_visible.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "tool/logger.hpp"
#include "ui/main_window.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget/scene/scene_item_selection_model.hpp"
#include "ui/widget/scene/scene_widget.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QAbstractItemModel>
#include <QDrag>
#include <stack>

namespace VTX::UI::Widget::Scene
{
	SceneItemWidget::SceneItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
		_registerEvent( Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE );
	}

	void SceneItemWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			_refreshSelection( *castedEvent.ptr );
		}
		else if ( p_event.name == Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<const Model::BaseModel> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<const Model::BaseModel> &>( p_event );

			_refreshCurrentItemInSelection( castedEvent.ptr );
		}
	}

	void SceneItemWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setUniformRowHeights( true );
		setHeaderHidden( true );
		setColumnCount( 1 );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
		setSelectionBehavior( SelectionBehavior::SelectRows );

		setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );

		setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );

		setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		setEditTriggers( EditTrigger::SelectedClicked );
		setExpandsOnDoubleClick( false );

		Model::BaseModel & vtxModel = MVC::MvcManager::get().getModel<Model::BaseModel>( getModelID() );
		setSelectionModel( new VTX::UI::Widget::Scene::SceneItemSelectionModel( &vtxModel, model(), this ) );

		_createTopLevelObject();
	}

	void SceneItemWidget::_setupSlots()
	{
		connect( this, &QTreeWidget::itemExpanded, this, &SceneItemWidget::_onItemExpanded );
		connect( this, &QTreeWidget::itemCollapsed, this, &SceneItemWidget::_onItemCollapsed );
		connect( this, &QTreeWidget::customContextMenuRequested, this, &SceneItemWidget::_onCustomContextMenuCalled );
	}

	void SceneItemWidget::localize() {}

	void SceneItemWidget::openRenameEditor( const Model::ID & p_modelID )
	{
		_openRenameEditor( *_findItemFromModelID( p_modelID ) );
	}
	void SceneItemWidget::_openRenameEditor( QTreeWidgetItem & p_target ) { editItem( &p_target ); }

	QTreeWidgetItem * SceneItemWidget::getLastVisibleItem()
	{
		return itemFromIndex( model()->index( model()->rowCount() - 1, model()->columnCount() - 1 ) );
	}

	void SceneItemWidget::updatePosInSceneHierarchy( const int p_position ) { _refreshSize(); };
	void SceneItemWidget::mousePressEvent( QMouseEvent * p_event )
	{
		QTreeWidget::mousePressEvent( p_event );

		if ( p_event->button() == Qt::LeftButton )
			_dragStartPosition = p_event->pos();

		p_event->accept();
	}

	void SceneItemWidget::keyPressEvent( QKeyEvent * p_event )
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

			if ( selection.getItems().size() == 1 && _itemCanBeRenamed( currentItem() ) )
			{
				_openRenameEditor( *currentItem() );
			}
		}
		else if ( p_event->key() == Qt::Key::Key_Up && itemFromIndex( currentIndex() ) == topLevelItem( 0 ) )
		{
			const VTX::UI::Widget::Scene::SceneWidget & sceneWidget
				= VTXApp::get().getMainWindow().getWidget<VTX::UI::Widget::Scene::SceneWidget>( ID::UI::Window::SCENE );
			SceneItemWidget * const previousTree = sceneWidget.getPreviousSceneItemWidgets( this );

			if ( previousTree != this )
			{
				QTreeWidgetItem * const previousTreeWidgetItem = previousTree->getLastVisibleItem();

				const bool appendToSelection = p_event->modifiers() == Qt::KeyboardModifier::ShiftModifier;
				_selectItemWithArrows( *previousTreeWidgetItem, appendToSelection );
			}
		}
		else if ( p_event->key() == Qt::Key::Key_Down && itemFromIndex( currentIndex() ) == getLastVisibleItem() )
		{
			const VTX::UI::Widget::Scene::SceneWidget & sceneWidget
				= VTXApp::get().getMainWindow().getWidget<VTX::UI::Widget::Scene::SceneWidget>( ID::UI::Window::SCENE );
			SceneItemWidget * const nextTree = sceneWidget.getNextSceneItemWidgets( this );

			if ( nextTree != this )
			{
				QTreeWidgetItem * const nextTreeWidgetItem = nextTree->topLevelItem( 0 );

				const bool appendToSelection = p_event->modifiers() == Qt::KeyboardModifier::ShiftModifier;
				_selectItemWithArrows( *nextTreeWidgetItem, appendToSelection );
			}
		}
		else
		{
			BaseManualWidget::keyPressEvent( p_event );
		}
	}

	void SceneItemWidget::dragEnterEvent( QDragEnterEvent * p_event )
	{
		BaseManualWidget::dragEnterEvent( p_event );
		if ( p_event->mimeData()->hasFormat(
				 VTX::UI::MimeType::getQStringMimeType( VTX::UI::MimeType::ApplicationMimeType::SCENE_ITEM ) ) )
			p_event->acceptProposedAction();
	}
	void SceneItemWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		BaseManualWidget::mouseMoveEvent( p_event );

		if ( !( p_event->buttons() & Qt::LeftButton ) )
			return;

		if ( !_canDragObjectAtPos( _dragStartPosition ) )
			return;

		if ( ( p_event->pos() - _dragStartPosition ).manhattanLength() < QApplication::startDragDistance() )
			return;

		QDrag *		drag	 = new QDrag( this );
		QMimeData * mimeData = _getDataForDrag();
		drag->setMimeData( mimeData );

		drag->exec( Qt::CopyAction | Qt::MoveAction );
	}

	void SceneItemWidget::_onItemExpanded( QTreeWidgetItem * const )
	{
		_refreshSize();

		// Add this because if the current object is in the expanded item, Qt auto-scroll on it and break the layout
		scrollToTop();
	}
	void SceneItemWidget::_onItemCollapsed( QTreeWidgetItem * const ) { _refreshSize(); }

	void SceneItemWidget::_onCustomContextMenuCalled( const QPoint & p_clicPos ) {}

	void SceneItemWidget::_refreshSelection( const Model::Selection & p_selection )
	{
		_enableSignals( false );
		QItemSelection selection = QItemSelection();
		_fillItemSelection( p_selection, selection );

		VTX::UI::Widget::Scene::SceneItemSelectionModel * const selectModel
			= dynamic_cast<VTX::UI::Widget::Scene::SceneItemSelectionModel * const>( selectionModel() );

		selectModel->refreshSelection( selection );

		if ( p_selection.getCurrentObject() == nullptr )
		{
			selectModel->setCurrentIndex( QModelIndex(), QItemSelectionModel::SelectionFlag::Current );
		}
		else
		{
			QTreeWidgetItem * const currentItem = _findItemFromModelID( p_selection.getCurrentObject()->getId() );

			if ( currentItem != nullptr )
			{
				selectModel->setCurrentIndex( indexFromItem( currentItem ),
											  QItemSelectionModel::SelectionFlag::Current );
			}
		}
		_enableSignals( true );
	}
	void SceneItemWidget::_fillItemSelection( const Model::Selection & p_selection,
											  QItemSelection &		   p_itemSelection ) {};

	void SceneItemWidget::_refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const bool p_visible )
	{
		_enableSignals( false );
		p_itemWidget->setCheckState( 0, Util::UI::getCheckState( p_visible ) );
		_enableSignals( true );
	}

	void SceneItemWidget::_refreshItemsVisibility()
	{
		_enableSignals( false );
		_refreshItemsVisibilityRecursive( *topLevelItem( 0 ) );
		_enableSignals( true );
	}
	void SceneItemWidget::_refreshItemsVisibilityRecursive( QTreeWidgetItem & p_widget )
	{
		const Model::ID	   itemID	 = _getModelIDFromItem( p_widget );
		const ID::VTX_ID & modelType = MVC::MvcManager::get().getModelTypeID( itemID );

		bool visibility;
		if ( modelType == ID::Model::MODEL_MOLECULE )
		{
			visibility = MVC::MvcManager::get().getModel<Model::Molecule>( itemID ).isVisible();
		}
		else if ( modelType == ID::Model::MODEL_CHAIN )
		{
			visibility = MVC::MvcManager::get().getModel<Model::Chain>( itemID ).isVisible();
		}
		else if ( modelType == ID::Model::MODEL_RESIDUE )
		{
			visibility = MVC::MvcManager::get().getModel<Model::Residue>( itemID ).isVisible();
		}
		else if ( modelType == ID::Model::MODEL_ATOM )
		{
			visibility = MVC::MvcManager::get().getModel<Model::Atom>( itemID ).isVisible();
		}
		else
		{
			visibility = false;
		}

		p_widget.setCheckState( 0, Util::UI::getCheckState( visibility ) );

		for ( int i = 0; i < p_widget.childCount(); i++ )
		{
			_refreshItemsVisibilityRecursive( *p_widget.child( i ) );
		}
	}

	void SceneItemWidget::_enableSignals( const bool p_enable )
	{
		if ( p_enable )
			_enableSignalCounter--;
		else
			_enableSignalCounter++;

		const bool isEnable = _enableSignalCounter == 0;
		blockSignals( !isEnable );
		setUpdatesEnabled( isEnable );
	}

	void SceneItemWidget::_createTopLevelObject()
	{
		const Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( getModelID() );

		QTreeWidgetItem * const topLevelItem = new QTreeWidgetItem();
		topLevelItem->setFlags( topLevelItem->flags() | Qt::ItemFlag::ItemIsEditable );

		topLevelItem->setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::Model::ID>( model.getId() ) );
		topLevelItem->setText( 0, QString::fromStdString( model.getDefaultName() ) );
		topLevelItem->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );

		_refreshItemVisibility( topLevelItem, true );

		addTopLevelItem( topLevelItem );
	}

	void SceneItemWidget::_selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append )
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::ID & itemModel = _getModelIDFromItem( p_itemToSelect );
		const ID::VTX_ID  itemType	= MVC::MvcManager::get().getModelTypeID( itemModel );

		p_itemToSelect.treeWidget()->setFocus( Qt::FocusReason::TabFocusReason );
		p_itemToSelect.treeWidget()->setCurrentItem( &p_itemToSelect );

		selectionModel.selectModel( MVC::MvcManager::get().getModel<Model::BaseModel>( itemModel ), p_append );
	}

	void SceneItemWidget::_refreshSize()
	{
		setMinimumHeight( _getMinimumHeight() );
		setMinimumWidth( sizeHintForColumn( 0 ) );
	}
	int SceneItemWidget::_getMinimumHeight() const
	{
		int nbItemDisplayed = 0;

		QModelIndex ptr		   = indexFromItem( topLevelItem( 0 ) );
		uint		childCount = 0;

		std::stack<std::pair<QModelIndex, int>> stack = std::stack<std::pair<QModelIndex, int>>();
		stack.push( std::pair<QModelIndex, int>( ptr, 0 ) );

		while ( stack.size() > 0 && nbItemDisplayed < 4 )
		{
			if ( !ptr.isValid() )
			{
				stack.pop();

				if ( stack.size() > 1 )
				{
					std::pair<QModelIndex, int> pair = stack.top();
					ptr								 = pair.first.model()->index( pair.second, 0, pair.first.parent() );
				}
			}
			else if ( itemFromIndex( ptr )->isHidden() )
			{
				childCount++;
				ptr = ptr.model()->index( childCount, 0, ptr.parent() );
			}
			else if ( isExpanded( ptr ) )
			{
				nbItemDisplayed++;
				stack.push( std::pair<QModelIndex, int>( ptr, childCount ) );
				ptr		   = ptr.model()->index( 0, 0, ptr );
				childCount = 0;
			}
			else
			{
				nbItemDisplayed++;
				childCount++;
				ptr = ptr.model()->index( childCount, 0, ptr.parent() );
			}
		}

		return nbItemDisplayed > 3 ? 0 : ( rowHeight( model()->index( 0, 0 ) ) * nbItemDisplayed );
	}

	bool SceneItemWidget::_itemCanBeRenamed( const QTreeWidgetItem * p_item ) { return true; }

	Model::ID SceneItemWidget::_getModelIDFromItem( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & dataID = p_item.data( 0, MODEL_ID_ROLE );
		return dataID.value<VTX::Model::ID>();
	}
	QTreeWidgetItem * SceneItemWidget::_findItemFromModelID( const Model::ID & p_id ) const
	{
		return _findItemFromModelIDRecursive( *topLevelItem( 0 ), p_id );
	}
	QTreeWidgetItem * SceneItemWidget::_findItemFromModelIDRecursive( QTreeWidgetItem & p_parent,
																	  const Model::ID & p_id ) const
	{
		if ( _getModelIDFromItem( p_parent ) == p_id )
			return &p_parent;

		for ( int i = 0; i < p_parent.childCount(); i++ )
		{
			QTreeWidgetItem * child = p_parent.child( i );
			QTreeWidgetItem * res	= _findItemFromModelIDRecursive( *child, p_id );

			if ( res != nullptr )
				return res;
		}

		return nullptr;
	}

	bool SceneItemWidget::_getItemExpandState( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & expandState = p_item.data( 0, EXPAND_STATE_ROLE );
		return expandState.isValid() && expandState.value<bool>();
	}

	void SceneItemWidget::_refreshCurrentItemInSelection( const Model::BaseModel * const p_obj )
	{
		_enableSignals( false );
		if ( p_obj == nullptr )
		{
			selectionModel()->setCurrentIndex( QModelIndex(), QItemSelectionModel::SelectionFlag::NoUpdate );
		}
		else
		{
			QTreeWidgetItem * const item = _findItemFromModelID( p_obj->getId() );
			if ( item != nullptr )
			{
				selectionModel()->setCurrentIndex( indexFromItem( item ), QItemSelectionModel::SelectionFlag::Current );
				VTXApp::get().getMainWindow().getWidget<SceneWidget>( ID::UI::Window::SCENE ).scrollToItem( *item );
			}
		}
		_enableSignals( true );
	}

} // namespace VTX::UI::Widget::Scene
