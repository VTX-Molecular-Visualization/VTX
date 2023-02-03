#include "scene_widget.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/tool/keys.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include "action/action_manager.hpp"
#include "action/scene.hpp"
#include "action/selection.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "vtx_app.hpp"
#include <QScrollBar>
#include <algorithm>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	SceneWidget::SceneWidget( QWidget * p_parent ) : QtDockablePanel()
	{
		_registerEvent( Event::Global::SCENE_ITEM_INDEXES_CHANGE );

		_registerEvent( Event::Global::SCENE_ITEM_ADDED );
		_registerEvent( Event::Global::SCENE_ITEM_REMOVED );
	}

	SceneWidget ::~SceneWidget()
	{
		for ( const std::pair<ID::VTX_ID, SceneItemWidgetInstancier *> & pairInstancier : _mapInstanciers )
		{
			delete pairInstancier.second;
		}

		_mapInstanciers.clear();
	}

	void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SCENE_ITEM_ADDED )
		{
			const Event::VTXEventPtr<Generic::BaseSceneItem> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Generic::BaseSceneItem> &>( p_event );

			instantiateSceneItemWidget( castedEvent.ptr );
		}
		else if ( p_event.name == Event::Global::SCENE_ITEM_REMOVED )
		{
			const Event::VTXEventPtr<Generic::BaseSceneItem> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Generic::BaseSceneItem> &>( p_event );

			Model::BaseModel & model
				= MVC::MvcManager::get().getModel<Model::BaseModel>( castedEvent.ptr->getModelID() );

			SceneItemWidget * const sceneItemWidget = getSceneItemWidgetFromModel( model );

			deleteSceneItemWidget( sceneItemWidget );
		}
		else if ( p_event.name == Event::Global::SCENE_ITEM_INDEXES_CHANGE )
		{
			_refreshItemIndex();
		}
	}

	void SceneWidget::registerSceneItemType( const ID::VTX_ID & p_type, SceneItemWidgetInstancier * const p_instancier )
	{
		_mapInstanciers[ p_type ] = p_instancier;
	}
	SceneItemWidget * SceneWidget::instantiateSceneItemWidget( Generic::BaseSceneItem * const p_sceneItem )
	{
		const int defaultPosition = _getDefaultIndex( *p_sceneItem );

		const ID::VTX_ID &		modelTypeID		= MVC::MvcManager::get().getModelTypeID( p_sceneItem->getModelID() );
		SceneItemWidget * const sceneItemWidget = _mapInstanciers[ modelTypeID ]->instantiateItem( p_sceneItem );

		const int index = int( _sceneWidgets.size() );
		_addWidgetInLayout( sceneItemWidget, index );
		_sceneWidgets.emplace_back( sceneItemWidget );
		sceneItemWidget->updatePosInSceneHierarchy( index );

		Object3D::Scene & scene = VTXApp::get().getScene();
		scene.changeModelPosition( *p_sceneItem, defaultPosition );

		return sceneItemWidget;
	}
	void SceneWidget::deleteSceneItemWidget( SceneItemWidget * const p_item )
	{
		_removeWidgetInLayout( p_item );

		const ID::VTX_ID & itemType = MVC::MvcManager::get().getModelTypeID( p_item->getModelID() );
		_mapInstanciers[ itemType ]->destroyItem( p_item );
	}

	void SceneWidget::_refreshItemIndex()
	{
		const Object3D::Scene & scene = VTXApp::get().getScene();

		for ( int i = 0; i < _sceneWidgets.size(); i++ )
		{
			const Generic::BaseSceneItem * item = scene.getItemAtPosition( i );
			if ( _sceneWidgets[ i ]->getModelID() != item->getModelID() )
			{
				const int previousItemIndex = _findItemIndex( item->getModelID(), i + 1 );
				_swapItems( i, previousItemIndex );
			}
		}
	}

	int SceneWidget::_findItemIndex( const Model::ID & p_modelID, const int p_startIndex ) const
	{
		for ( int i = p_startIndex; i < _sceneWidgets.size(); i++ )
		{
			if ( _sceneWidgets[ i ]->getModelID() == p_modelID )
				return i;
		}
		return -1;
	}
	void SceneWidget::_swapItems( const int p_index1, const int p_index2 )
	{
		SceneItemWidget * const tmp = _sceneWidgets[ p_index1 ];
		_sceneWidgets[ p_index1 ]	= _sceneWidgets[ p_index2 ];
		_sceneWidgets[ p_index2 ]	= tmp;

		_layout->removeWidget( _sceneWidgets[ p_index1 ] );
		_layout->removeWidget( _sceneWidgets[ p_index2 ] );

		if ( p_index1 < p_index2 )
		{
			_addWidgetInLayout( _sceneWidgets[ p_index1 ], p_index1 );
			_addWidgetInLayout( _sceneWidgets[ p_index2 ], p_index2 );
		}
		else
		{
			_addWidgetInLayout( _sceneWidgets[ p_index2 ], p_index2 );
			_addWidgetInLayout( _sceneWidgets[ p_index1 ], p_index1 );
		}

		_sceneWidgets[ p_index1 ]->updatePosInSceneHierarchy( p_index1 );
		_sceneWidgets[ p_index2 ]->updatePosInSceneHierarchy( p_index2 );
	}

	SceneItemWidget * SceneWidget::getPreviousSceneItemWidgets( SceneItemWidget * p_item ) const
	{
		SceneItemWidget * res		= nullptr;
		bool			  itemFound = false;

		for ( std::vector<SceneItemWidget *>::const_iterator it = _sceneWidgets.begin(); it != _sceneWidgets.end();
			  it++ )
		{
			if ( p_item == *it )
			{
				if ( res == nullptr )
					res = *it;

				itemFound = true;
				break;
			}

			res = *it;
		}

		return itemFound ? res : nullptr;
	}
	SceneItemWidget * SceneWidget::getNextSceneItemWidgets( SceneItemWidget * p_item ) const
	{
		SceneItemWidget * res		= nullptr;
		bool			  itemFound = false;

		for ( std::vector<SceneItemWidget *>::const_reverse_iterator it = _sceneWidgets.rbegin();
			  it != _sceneWidgets.rend();
			  it++ )
		{
			if ( p_item == *it )
			{
				if ( res == nullptr )
					res = *it;

				itemFound = true;
				break;
			}

			res = *it;
		}

		return itemFound ? res : nullptr;
	}

	SceneItemWidget * SceneWidget::getSceneItemWidgetFromModel( const Model::BaseModel & p_model ) const
	{
		for ( SceneItemWidget * const sceneItem : _sceneWidgets )
		{
			if ( sceneItem->containsModel( p_model ) )
				return sceneItem;
		}

		return nullptr;
	}

	void SceneWidget::openRenameEditor( const Model::ID & p_itemID ) const
	{
		for ( SceneItemWidget * const sceneWidget : _sceneWidgets )
		{
			if ( sceneWidget->getModelID() == p_itemID )
			{
				sceneWidget->openRenameEditor( p_itemID );
			}
		}
	}

	void SceneWidget::scrollToItem( const QTreeWidgetItem & p_item )
	{
		const QTreeWidget & tree				 = *( p_item.treeWidget() );
		const QRect			rectItem			 = tree.visualItemRect( &p_item );
		const QPoint		localRectPos		 = QPoint( rectItem.x(), rectItem.y() );
		const QPoint		globalRectPos		 = tree.mapToGlobal( localRectPos );
		const QPoint		localToScrollRectPos = _scrollAreaContent->mapFromGlobal( globalRectPos );

		const int scrollBarPosition = _scrollArea->verticalScrollBar()->sliderPosition();
		const int viewportHeight	= _scrollArea->viewport()->height();

		if ( localToScrollRectPos.y() < scrollBarPosition )
		{
			_scrollArea->verticalScrollBar()->setSliderPosition( localToScrollRectPos.y() );
		}
		else if ( localToScrollRectPos.y() > ( scrollBarPosition + viewportHeight - rectItem.height() ) )
		{
			_scrollArea->verticalScrollBar()->setSliderPosition( localToScrollRectPos.y() - viewportHeight
																 + rectItem.height() );
		}
	}

	int SceneWidget::_getDefaultIndex( const Generic::BaseSceneItem & p_item ) const
	{
		if ( _sceneWidgets.size() == 0 )
			return 0;

		const Model::ID & sceneDefaultPathID = VTXApp::get().getScene().getDefaultPath()->getId();

		// TODO : Better management of section in scene view.
		if ( p_item.getModelID() != sceneDefaultPathID )
		{
			const std::vector<SceneItemWidget *>::const_reverse_iterator lastItemIt = _sceneWidgets.crbegin();

			if ( ( *lastItemIt )->getModelID() == sceneDefaultPathID )
			{
				return int( _sceneWidgets.size() - 1 );
			}
		}

		return int( _sceneWidgets.size() );
	}

	void SceneWidget::_addWidgetInLayout( QWidget * const p_widget, const int p_index )
	{
		_layout->insertWidget( p_index, p_widget, 1 );

		if ( _sceneWidgets.size() > 0 )
			setTabOrder( *_sceneWidgets.rbegin(), p_widget );
	}
	void SceneWidget::_removeWidgetInLayout( SceneItemWidget * const p_sceneItemWidget )
	{
		const int itemID	 = p_sceneItemWidget->getModelID();
		bool	  itemErased = false;
		int		  counter	 = 0;
		while ( counter < _sceneWidgets.size() )
		{
			SceneItemWidget * const item = _sceneWidgets[ counter ];
			if ( itemErased )
			{
				item->updatePosInSceneHierarchy( counter - 1 );
				counter++;
			}
			else if ( item->getModelID() == itemID )
			{
				_sceneWidgets.erase( _sceneWidgets.begin() + counter );
				itemErased = true;
			}
			else
			{
				counter++;
			}
		}

		_layout->removeWidget( p_sceneItemWidget );
	}

	void SceneWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_scrollAreaContent = new QWidget( this );

		_layout = new QVBoxLayout( _scrollAreaContent );
		_layout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_layout->setSpacing( Style::SCENE_SPACE_BETWEEN_ITEMS );
		_layout->addStretch( 1000 );
		_layout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new QT::Widget::CustomWidget::DockWindowMainWidget<QScrollArea>(
			Style::SCENE_PREFERRED_SIZE, Style::SCENE_MINIMUM_SIZE, this );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );

		_scrollArea->setSizePolicy( sizePolicy );
		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->setWidget( _scrollAreaContent );
		_scrollArea->setWidgetResizable( true );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );

		setWidget( _scrollArea );

		setAcceptDrops( true );
	}

	void SceneWidget::_setupSlots() {}

	void SceneWidget::localize()
	{
		this->setWindowTitle( "Scene" );
		// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
	}
	void SceneWidget::mousePressEvent( QMouseEvent * p_event )
	{
		// Click on header
		if ( !_scrollAreaContent->rect().contains( _scrollAreaContent->mapFromGlobal( p_event->globalPos() ) ) )
		{
			BaseManualWidget::mousePressEvent( p_event );
			return;
		}

		if ( p_event->buttons() & ( Qt::MouseButton::LeftButton | Qt::MouseButton::RightButton ) )
		{
			Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
			if ( !selectionModel.isEmpty() )
			{
				VTX_ACTION( new Action::Selection::ClearSelection( selectionModel ) );
			}

			p_event->accept();
		}

		if ( p_event->buttons() & Qt::MouseButton::RightButton )
		{
			Object3D::Scene & scene = VTXApp::get().getScene();

			QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::SCENE, p_event->globalPos() );

			p_event->accept();
		}
	}
	void SceneWidget::dragEnterEvent( QDragEnterEvent * p_event )
	{
		BaseManualWidget::dragEnterEvent( p_event );

		const bool draggedObjectIsModel
			= UI::MimeType::checkApplicationDataType( p_event->mimeData(), UI::MimeType::ApplicationMimeType::MODEL );

		if ( draggedObjectIsModel )
		{
			const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

			if ( modelData.getDragSource() == UI::MimeType::DragSource::SCENE_VIEW )
			{
				p_event->acceptProposedAction();
			}
		}
	}
	void SceneWidget::mouseMoveEvent( QMouseEvent * p_event ) { BaseManualWidget::mouseMoveEvent( p_event ); }

	void SceneWidget::dropEvent( QDropEvent * p_event )
	{
		BaseManualWidget::dropEvent( p_event );

		const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

		std::vector<Model::ID> droppedModelIDs = std::vector<Model::ID>();

		if ( modelData.getTypeID() == ID::Model::MODEL_SELECTION )
		{
			const Model::Selection & selection
				= MVC::MvcManager::get().getModel<Model::Selection>( modelData.getModelID() );

			droppedModelIDs.reserve( selection.getItems().size() );
			for ( const Model::ID & id : selection.getItems() )
			{
				droppedModelIDs.emplace_back( id );
			}
		}
		else
		{
			droppedModelIDs.resize( 1 );
			droppedModelIDs[ 0 ] = modelData.getModelID();
		}

		std::vector<const Generic::BaseSceneItem *> droppedItems = std::vector<const Generic::BaseSceneItem *>();
		droppedItems.reserve( droppedModelIDs.size() );

		for ( const Model::ID & droppedModelId : droppedModelIDs )
		{
			SceneItemWidget * sceneItemWidget = nullptr;
			int				  sceneItemIndex  = 0;
			for ( SceneItemWidget * const item : _sceneWidgets )
			{
				if ( item->getModelID() == droppedModelId )
				{
					sceneItemWidget = item;
					break;
				}
				sceneItemIndex++;
			}

			const Generic::BaseSceneItem & sceneItem = _sceneWidgets[ sceneItemIndex ]->getBaseSceneItem();
			droppedItems.emplace_back( &sceneItem );
		}

		const int eventPosY = _scrollAreaContent->mapFrom( this, p_event->pos() ).y();
		int		  newIndex	= (int)_sceneWidgets.size();

		for ( int i = 0; i < _sceneWidgets.size(); i++ )
		{
			const int itemPosY = _sceneWidgets[ i ]->pos().y() + ( _sceneWidgets[ i ]->height() / 2 );
			if ( itemPosY > eventPosY )
			{
				newIndex = i;
				break;
			}
		}

		p_event->acceptProposedAction();

		VTX_ACTION( new Action::Scene::ChangeItemIndex( droppedItems, newIndex ) );
	}
} // namespace VTX::UI::QT::Tool::Scene::Widget
