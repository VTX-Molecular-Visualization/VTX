#include "path_scene_view.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "action/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/widget/scene/scene_item_selection_model.hpp"
#include "ui/widget/scene/scene_widget.hpp"
#include "ui/widget_factory.hpp"
#include "util/string.hpp"
#include <QScrollBar>

namespace VTX::View::UI::Widget
{
	PathSceneView::PathSceneView( Model::Path * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Path>( p_model ), SceneItemWidget( p_parent )
	{
		_registerEvent( Event::Global::VIEWPOINT_ADDED );
		_registerEvent( Event::Global::VIEWPOINT_REMOVED );
	}
	PathSceneView::~PathSceneView() {}

	void PathSceneView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE ) {}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			topLevelItem( 0 )->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
		}
	}
	void PathSceneView::receiveEvent( const Event::VTXEvent & p_event )
	{
		SceneItemWidget::receiveEvent( p_event );

		if ( p_event.name == Event::Global::VIEWPOINT_ADDED )
		{
			const Event::VTXEventPtr<Model::Viewpoint> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Viewpoint> &>( p_event );

			_addViewpoint( castedEvent.ptr );
		}
		else if ( p_event.name == Event::Global::VIEWPOINT_REMOVED )
		{
			const Event::VTXEventPtr<Model::Viewpoint> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Viewpoint> &>( p_event );

			_removeViewpoint( castedEvent.ptr );
		}
	}

	QTreeWidgetItem * PathSceneView::getLastVisibleItem()
	{
		QTreeWidgetItem * pathItem = topLevelItem( 0 );

		if ( pathItem->isExpanded() )
			return pathItem->child( pathItem->childCount() - 1 );
		else
			return pathItem;
	}

	void PathSceneView::_selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append )
	{
		// TODO
		// Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
		// selectionModel.selectViewpoint( *_model );
	}

	void PathSceneView::_setupUi( const QString & p_name ) { SceneItemWidget::_setupUi( p_name ); }
	void PathSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &PathSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &PathSceneView::_onItemDoubleClicked );
	}
	void PathSceneView::localize() { SceneItemWidget::localize(); }

	void PathSceneView::_createTopLevelObject()
	{
		SceneItemWidget::_createTopLevelObject();
		topLevelItem( 0 )->setText( 0, QString::fromStdString( Style::VIEWPOINT_GROUP_NAME ) );
	}

	void PathSceneView::_fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection )
	{
		std::set<Model::Viewpoint *> viewpoints = std::set<Model::Viewpoint *>();
		p_selection.getItemsOfType( ID::Model::MODEL_VIEWPOINT, viewpoints );

		bool pathItemAdded = false;

		for ( Model::Viewpoint * const viewpoint : viewpoints )
		{
			if ( viewpoint->getPathPtr() != _model )
				continue;

			if ( !pathItemAdded )
			{
				QTreeWidgetItem * const pathItem = topLevelItem( 0 );
				p_itemSelection.append( QItemSelectionRange( indexFromItem( pathItem ) ) );
				pathItemAdded = true;
			}

			QTreeWidgetItem * const viewpointItem = _findItemFromModelID( viewpoint->getId() );
			p_itemSelection.append( QItemSelectionRange( indexFromItem( viewpointItem ) ) );
		}
	}

	void PathSceneView::mouseMoveEvent( QMouseEvent * p_event )
	{
		setSelectionMode( QAbstractItemView::ContiguousSelection );
		SceneItemWidget::mouseMoveEvent( p_event );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
	}

	void PathSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			const Model::ID idTarget = p_item->data( 0, MODEL_ID_ROLE ).value<Model::ID>();
			if ( MVC::MvcManager::get().getModelTypeID( idTarget ) == ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & viewpoint = MVC::MvcManager::get().getModel<Model::Viewpoint>( idTarget );
				std::string		   itemTxt	 = p_item->text( 0 ).toStdString();

				if ( itemTxt != viewpoint.getDefaultName() )
				{
					_reformatName( itemTxt );

					if ( itemTxt.empty() )
					{
						itemTxt = viewpoint.getDefaultName();
					}
					else if ( itemTxt != viewpoint.getDefaultName() )
					{
						VTX_ACTION( new Action::Viewpoint::Rename( viewpoint, itemTxt ) );
					}

					const bool oldSignalState = blockSignals( true );
					p_item->setText( 0, QString::fromStdString( itemTxt ) );
					blockSignals( oldSignalState );

					return;
				}
			}
		}
	}
	void PathSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		if ( p_column == 0 )
		{
			const Model::ID idTarget = p_item->data( 0, MODEL_ID_ROLE ).value<Model::ID>();
			if ( MVC::MvcManager::get().getModelTypeID( idTarget ) == ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & viewpoint  = MVC::MvcManager::get().getModel<Model::Viewpoint>( idTarget );
				Object3D::Camera & mainCamera = VTXApp::get().getScene().getCamera();

				VTX_ACTION( new Action::Viewpoint::GoTo( viewpoint, mainCamera ) );
			}
		}
	}
	void PathSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const Model::ID & itemID = _getModelIDFromItem( *targetedItem );

		if ( MVC::MvcManager::get().getModelTypeID( itemID ) != ID::Model::MODEL_VIEWPOINT )
			return;

		Model::Viewpoint & viewpointTargeted = MVC::MvcManager::get().getModel<Model::Viewpoint>( itemID );

		const QPoint globalClicPos = mapToGlobal( p_clicPos );

		Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();

		if ( selection.isModelSelected( viewpointTargeted ) )
		{
			VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
				= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
					VTX::UI::ContextualMenu::Menu::Selection );
			selectionContextualMenu->setFocusedTarget( &viewpointTargeted );
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
		}
		else
		{
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Viewpoint, &viewpointTargeted, globalClicPos );
		}
	}

	void PathSceneView::_reformatName( std::string & p_name ) const
	{
		// Trim
		Util::String::trim( p_name );

		// Forbid line feed
		const size_t firstLinefeedIndex = p_name.find_first_of( '\n' );
		if ( firstLinefeedIndex != std::string::npos )
			p_name.erase( firstLinefeedIndex );

		// Clamp size
		if ( p_name.size() > Style::MOLECULE_NAME_MAXIMUM_SIZE )
			p_name = p_name.substr( 0, Style::MOLECULE_NAME_MAXIMUM_SIZE );
	}

	QMimeData * PathSceneView::_getDataForDrag() { return VTX::UI::MimeType::generatePathData( *_model ); }

	void PathSceneView::updatePosInSceneHierarchy( const int p_position )
	{
		SceneItemWidget::updatePosInSceneHierarchy( p_position );

		// TODO Manage item scene position
		//_model->getConfiguration().sceneIndex = p_position;
	}

	QTreeWidgetItem * PathSceneView::_itemFromViewpoint( const Model::Viewpoint & p_viewpoint ) const
	{
		const Model::ID & viewpointID = p_viewpoint.getId();

		for ( int i = 0; i < topLevelItem( 0 )->childCount(); i++ )
		{
			QTreeWidgetItem * const item   = topLevelItem( 0 )->child( i );
			const Model::ID &		itemID = item->data( 0, MODEL_ID_ROLE ).value<Model::ID>();
			if ( viewpointID == itemID )
				return item;
		}

		return nullptr;
	}

	void PathSceneView::_addViewpoint( const Model::Viewpoint * const p_viewpoint )
	{
		QTreeWidgetItem * const viewpointItem = new QTreeWidgetItem();
		viewpointItem->setFlags( viewpointItem->flags() | Qt::ItemFlag::ItemIsEditable );

		viewpointItem->setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::Model::ID>( p_viewpoint->getId() ) );
		viewpointItem->setText( 0, QString::fromStdString( p_viewpoint->getDefaultName() ) );
		viewpointItem->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_viewpoint->getTypeId() ) );

		topLevelItem( 0 )->addChild( viewpointItem );

		_refreshSize();
	}
	void PathSceneView::_removeViewpoint( const Model::Viewpoint * const p_viewpoint )
	{
		QTreeWidgetItem * const viewpointItem = _itemFromViewpoint( *p_viewpoint );
		if ( viewpointItem != nullptr )
			topLevelItem( 0 )->removeChild( viewpointItem );

		_refreshSize();
	}

} // namespace VTX::View::UI::Widget