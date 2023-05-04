#include "ui/qt/tool/scene/widget/scene_item_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/mime_type.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/scene/widget/scene_item_selection_model.hpp"
#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include "ui/qt/util.hpp"
#include <QAbstractItemModel>
#include <QDrag>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/category.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/event/global.hpp>
#include <app/mvc.hpp>
#include <app/component/generic/base_visible.hpp>
#include <stack>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	SceneItemWidget::SceneItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ), DraggableItem( this )
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_CHANGE );
		_registerEvent( VTX::App::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE );
	}

	void SceneItemWidget::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::SELECTION_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<const App::Application::Selection::SelectionModel *> & castedEvent
				= dynamic_cast<
					const VTX::App::Core::Event::VTXEventArg<const App::Application::Selection::SelectionModel *> &>(
					p_event );

			_refreshSelection( *castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<const App::Core::Model::BaseModel *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<const App::Core::Model::BaseModel *> &>(
					p_event );

			_refreshCurrentItemInSelection( castedEvent.get() );
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

		App::Core::Model::BaseModel & vtxModel
			= VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( getModelID() );
		setSelectionModel( new SceneItemSelectionModel( &vtxModel, model(), this ) );

		_createTopLevelObject();
	}

	void SceneItemWidget::_setupSlots()
	{
		connect( this, &QTreeWidget::itemExpanded, this, &SceneItemWidget::_onItemExpanded );
		connect( this, &QTreeWidget::itemCollapsed, this, &SceneItemWidget::_onItemCollapsed );
		connect( this, &QTreeWidget::customContextMenuRequested, this, &SceneItemWidget::_onCustomContextMenuCalled );
	}

	void SceneItemWidget::localize() {}

	void SceneItemWidget::openRenameEditor( const App::Core::Model::ID & p_modelID )
	{
		QTreeWidgetItem * const itemWidget = _findItemFromModelID( p_modelID );

		if ( itemWidget != nullptr )
			_openRenameEditor( *itemWidget );
	}
	void SceneItemWidget::_openRenameEditor( QTreeWidgetItem & p_target ) { editItem( &p_target ); }

	std::vector<App::Core::Model::ID> SceneItemWidget::getAllItemsFrom(
		const App::Core::Model::BaseModel & p_model ) const
	{
		// Default return for scene item without subitems
		return { getModelID() };
	}
	std::vector<App::Core::Model::ID> SceneItemWidget::getAllItemsTo(
		const App::Core::Model::BaseModel & p_model ) const
	{
		// Default return for scene item without subitems
		return { getModelID() };
	}

	QTreeWidgetItem * SceneItemWidget::getLastVisibleItem()
	{
		return itemFromIndex( model()->index( model()->rowCount() - 1, model()->columnCount() - 1 ) );
	}

	void SceneItemWidget::updatePosInSceneHierarchy( const int p_position ) { _refreshSize(); };

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
			const App::Application::Selection::SelectionModel & selection
				= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

			if ( selection.getItems().size() == 1 && _itemCanBeRenamed( currentItem() ) )
			{
				_openRenameEditor( *currentItem() );
			}
		}
		else if ( p_event->key() == Qt::Key::Key_Up && itemFromIndex( currentIndex() ) == topLevelItem( 0 ) )
		{
			const SceneWidget & sceneWidget
				= *( QT_APP()->getMainWindow().getPanel<SceneWidget>( Tool::SCENE_WINDOW_KEY ) );
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
			const SceneWidget & sceneWidget
				= *( QT_APP()->getMainWindow().getPanel<SceneWidget>( Tool::SCENE_WINDOW_KEY ) );
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

		const bool draggedObjectIsModel
			= QT::MimeType::checkApplicationDataType( p_event->mimeData(), MimeType::ApplicationMimeType::MODEL );

		if ( draggedObjectIsModel )
		{
			const MimeType::ModelData modelData = MimeType::getModelData( p_event->mimeData() );

			if ( modelData.getDragSource() == MimeType::DragSource::SCENE_VIEW )
			{
				p_event->acceptProposedAction();
			}
		}
	}

	void SceneItemWidget::mousePressEvent( QMouseEvent * p_event )
	{
		BaseManualWidget::mousePressEvent( p_event );

		// Ignore event to allow eventfilter of DraggableItem to receive mousePressEvent.
		p_event->ignore();
	}

	void SceneItemWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		setSelectionMode( QAbstractItemView::ContiguousSelection );
		BaseManualWidget::mouseMoveEvent( p_event );

		setSelectionMode( QAbstractItemView::ExtendedSelection );

		// Ignore event to allow eventfilter of DraggableItem to receive mouseMoveEvent.
		p_event->ignore();
	}

	void SceneItemWidget::_onItemExpanded( QTreeWidgetItem * const )
	{
		_refreshSize();

		// Add this because if the current object is in the expanded item, Qt auto-scroll on it and break the layout
		scrollToTop();
	}
	void SceneItemWidget::_onItemCollapsed( QTreeWidgetItem * const ) { _refreshSize(); }

	void SceneItemWidget::_onCustomContextMenuCalled( const QPoint & p_clicPos ) {}

	void SceneItemWidget::_refreshSelection( const App::Application::Selection::SelectionModel & p_selection )
	{
		_enableSignals( false );
		QItemSelection selection = QItemSelection();
		_fillItemSelection( p_selection, selection );

		SceneItemSelectionModel * const selectModel = dynamic_cast<SceneItemSelectionModel * const>( selectionModel() );

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
	void SceneItemWidget::_fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
											  QItemSelection &									  p_itemSelection ) {};

	void SceneItemWidget::_refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const bool p_visible )
	{
		// TMP add null check while category is not finished
		if ( p_itemWidget == nullptr )
			return;

		_enableSignals( false );
		p_itemWidget->setCheckState( 0, QT::Util::getCheckState( p_visible ) );
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
		const App::Core::Model::ID itemID	 = _getModelIDFromItem( p_widget );
		const App::VTX_ID &		   modelType = VTX::MVC_MANAGER().getModelTypeID( itemID );

		bool visibility;
		if ( modelType == App::ID::Model::MODEL_MOLECULE )
		{
			visibility = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( itemID ).isVisible();
		}
		else if ( modelType == App::ID::Model::MODEL_CATEGORY )
		{
			visibility = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( itemID ).isVisible();
		}
		else if ( modelType == App::ID::Model::MODEL_CHAIN )
		{
			visibility = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( itemID ).isVisible();
		}
		else if ( modelType == App::ID::Model::MODEL_RESIDUE )
		{
			visibility = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( itemID ).isVisible();
		}
		else if ( modelType == App::ID::Model::MODEL_ATOM )
		{
			visibility = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( itemID ).isVisible();
		}
		else
		{
			visibility = false;
		}

		p_widget.setCheckState( 0, QT::Util::getCheckState( visibility ) );

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
		const App::Core::Model::BaseModel & model
			= VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( getModelID() );

		QTreeWidgetItem * const topLevelItem = new QTreeWidgetItem();
		topLevelItem->setFlags( topLevelItem->flags() | Qt::ItemFlag::ItemIsEditable );

		topLevelItem->setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::App::Core::Model::ID>( model.getId() ) );
		topLevelItem->setText( 0, QString::fromStdString( model.getDefaultName() ) );
		topLevelItem->setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );

		_refreshItemVisibility( topLevelItem, true );

		addTopLevelItem( topLevelItem );
	}

	void SceneItemWidget::_selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append )
	{
		App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

		const App::Core::Model::ID & itemModel = _getModelIDFromItem( p_itemToSelect );
		const App::VTX_ID			 itemType  = VTX::MVC_MANAGER().getModelTypeID( itemModel );

		p_itemToSelect.treeWidget()->setFocus( Qt::FocusReason::TabFocusReason );
		p_itemToSelect.treeWidget()->setCurrentItem( &p_itemToSelect );

		selectionModel.selectModel( VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( itemModel ), p_append );
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

	App::Core::Model::ID SceneItemWidget::_getModelIDFromItem( const QTreeWidgetItem & p_item ) const
	{
		const QVariant & dataID = p_item.data( 0, MODEL_ID_ROLE );
		return dataID.value<VTX::App::Core::Model::ID>();
	}
	QTreeWidgetItem * SceneItemWidget::_findItemFromModelID( const App::Core::Model::ID & p_id ) const
	{
		return _findItemFromModelIDRecursive( *topLevelItem( 0 ), p_id );
	}
	QTreeWidgetItem * SceneItemWidget::_findItemFromModelIDRecursive( QTreeWidgetItem &			   p_parent,
																	  const App::Core::Model::ID & p_id ) const
	{
		if ( _getModelIDFromItem( p_parent ) == p_id )
			return &p_parent;

		if ( !p_parent.isExpanded() )
			return nullptr;

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

	QMimeData * SceneItemWidget::_getDataForDrag() const
	{
		const App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		const bool isModelSelected = selectionModel.isModelSelected( getModelID() );

		const App::Core::Model::BaseModel * const modelDragged
			= isModelSelected ? &( selectionModel )
							  : &( VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( getModelID() ) );

		return MimeType::generateMimeDataFromModel( *modelDragged, MimeType::DragSource::SCENE_VIEW );
	}

	void SceneItemWidget::_refreshCurrentItemInSelection( const App::Core::Model::BaseModel * const p_obj )
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
				QT_APP()->getMainWindow().getPanel<SceneWidget>( Tool::SCENE_WINDOW_KEY )->scrollToItem( *item );
			}
		}
		_enableSignals( true );
	}

} // namespace VTX::UI::QT::Tool::Scene::Widget
