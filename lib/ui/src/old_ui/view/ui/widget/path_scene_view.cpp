#include "ui/old_ui/view/ui/widget/path_scene_view.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/contextual_menu.hpp"
#include "ui/old_ui/ui/mime_type.hpp"
#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/old_ui/ui/widget/scene/scene_item_selection_model.hpp"
#include "ui/old_ui/ui/widget/scene/scene_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/viewpoint.hpp"
#include <QScrollBar>
#include <app/action/selection.hpp>
#include <app/action/viewpoint.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/event/global.hpp>
#include <app/mvc.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::View::UI::Widget
{
	PathSceneView::PathSceneView( App::Component::Video::Path * const p_model, QWidget * const p_parent ) :
		App::Core::View::BaseView<App::Component::Video::Path>( p_model ), SceneItemWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::VIEWPOINT_ADDED );
		_registerEvent( VTX::App::Event::Global::VIEWPOINT_REMOVED );
	}
	PathSceneView::~PathSceneView() {}

	void PathSceneView::notify( const VTX::App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Event::Model::DATA_CHANGE ) {}
		else if ( p_event->name == VTX::App::Event::Model::DISPLAY_NAME_CHANGE )
		{
			topLevelItem( 0 )->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
		}
	}
	void PathSceneView::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		SceneItemWidget::receiveEvent( p_event );

		if ( p_event.name == VTX::App::Event::Global::VIEWPOINT_ADDED )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Component::Object3D::Viewpoint *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Component::Object3D::Viewpoint *> &>(
					p_event );

			_addViewpoint( castedEvent.get() );
			topLevelItem( 0 )->setExpanded( true );
		}
		else if ( p_event.name == VTX::App::Event::Global::VIEWPOINT_REMOVED )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Component::Object3D::Viewpoint *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Component::Object3D::Viewpoint *> &>(
					p_event );

			_removeViewpoint( castedEvent.get() );
		}
	}

	bool PathSceneView::containsModel( const App::Core::Model::BaseModel & p_model ) const
	{
		const VTX::App::VTX_ID &			modelTypeID = p_model.getTypeId();
		const App::Component::Video::Path * linkedPath;

		if ( modelTypeID == App::ID::Model::MODEL_PATH )
			linkedPath = static_cast<const App::Component::Video::Path *>( &p_model );
		else if ( modelTypeID == App::ID::Model::MODEL_VIEWPOINT )
			linkedPath = static_cast<const App::Component::Object3D::Viewpoint *>( &p_model )->getPathPtr();
		else // Not a valid type. Return false.
			return false;

		return linkedPath == _model;
	}

	std::vector<App::Core::Model::ID> PathSceneView::getAllItemsFrom(
		const App::Core::Model::BaseModel & p_model ) const
	{
		const App::VTX_ID & typeID = p_model.getTypeId();

		if ( typeID == App::ID::Model::MODEL_PATH )
		{
			return SceneItemWidget::getAllItemsFrom( p_model );
		}

		std::vector<App::Core::Model::ID> res;

		if ( p_model.getTypeId() == App::ID::Model::MODEL_VIEWPOINT )
		{
			for ( size_t i = _model->getViewpoints().size() - 1; i <= 0; i-- )
			{
				const App::Component::Object3D::Viewpoint * const viewpoint = _model->getViewpoints()[ i ];
				res.emplace_back( viewpoint->getId() );

				if ( viewpoint->getId() == p_model.getId() )
					break;
			}
		}

		return res;
	}
	std::vector<App::Core::Model::ID> PathSceneView::getAllItemsTo( const App::Core::Model::BaseModel & p_model ) const
	{
		const App::VTX_ID & typeID = p_model.getTypeId();

		if ( typeID == App::ID::Model::MODEL_PATH )
		{
			return SceneItemWidget::getAllItemsFrom( p_model );
		}

		std::vector<App::Core::Model::ID> res;

		if ( p_model.getTypeId() == App::ID::Model::MODEL_VIEWPOINT )
		{
			for ( int i = 0; i < _model->getViewpoints().size(); i++ )
			{
				const App::Component::Object3D::Viewpoint * const viewpoint = _model->getViewpoints()[ i ];
				res.emplace_back( viewpoint->getId() );

				if ( viewpoint->getId() == p_model.getId() )
					break;
			}
		}

		return res;
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
		// App::Application::Selection::SelectionModel & selectionModel =
		// App::Application::Selection::SelectionManager::get().getSelectionModel(); selectionModel.selectViewpoint(
		// *_model );
	}

	void PathSceneView::_setupUi( const QString & p_name )
	{
		SceneItemWidget::_setupUi( p_name );

		for ( const App::Component::Object3D::Viewpoint * const viewpoint : _model->getViewpoints() )
			_addViewpoint( viewpoint );
	}
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
		topLevelItem( 0 )->setText( 0, QString::fromStdString( VTX::UI::Style::VIEWPOINT_GROUP_NAME ) );
	}

	void PathSceneView::_fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
											QItemSelection &									p_itemSelection )
	{
		std::set<App::Component::Object3D::Viewpoint *> viewpoints = std::set<App::Component::Object3D::Viewpoint *>();
		p_selection.getItemsOfType( App::ID::Model::MODEL_VIEWPOINT, viewpoints );

		bool pathItemAdded = false;

		for ( const App::Component::Object3D::Viewpoint * const viewpoint : viewpoints )
		{
			if ( viewpoint->getPathPtr() != _model )
				continue;

			if ( !pathItemAdded )
			{
				const QTreeWidgetItem * const pathItem = topLevelItem( 0 );
				p_itemSelection.append( QItemSelectionRange( indexFromItem( pathItem ) ) );
				pathItemAdded = true;
			}

			const QTreeWidgetItem * const viewpointItem = _findItemFromModelID( viewpoint->getId() );

			if ( viewpointItem != nullptr )
				p_itemSelection.append( QItemSelectionRange( indexFromItem( viewpointItem ) ) );
		}
	}

	void PathSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			const App::Core::Model::ID idTarget = p_item->data( 0, MODEL_ID_ROLE ).value<App::Core::Model::ID>();
			if ( VTX::MVC_MANAGER().getModelTypeID( idTarget ) == App::ID::Model::MODEL_VIEWPOINT )
			{
				App::Component::Object3D::Viewpoint & viewpoint
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( idTarget );
				std::string itemTxt = p_item->text( 0 ).toStdString();

				if ( itemTxt != viewpoint.getDefaultName() )
				{
					_reformatName( itemTxt );

					if ( itemTxt.empty() )
					{
						itemTxt = viewpoint.getDefaultName();
					}
					else if ( itemTxt != viewpoint.getDefaultName() )
					{
						VTX_ACTION( new App::Action::Viewpoint::Rename( viewpoint, itemTxt ) );
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
			const App::Core::Model::ID idTarget = p_item->data( 0, MODEL_ID_ROLE ).value<App::Core::Model::ID>();
			if ( VTX::MVC_MANAGER().getModelTypeID( idTarget ) == App::ID::Model::MODEL_VIEWPOINT )
			{
				App::Component::Object3D::Viewpoint & viewpoint
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( idTarget );
				App::Component::Render::Camera & mainCamera = App::VTXApp::get().getScene().getCamera();

				VTX_ACTION( new VTX::UI::QT::Action::Viewpoint::GoTo( viewpoint, mainCamera ) );
			}
		}
	}
	void PathSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const App::Core::Model::ID & itemID = _getModelIDFromItem( *targetedItem );

		const QPoint globalClicPos = mapToGlobal( p_clicPos );

		if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == App::ID::Model::MODEL_PATH )
		{
			App::Component::Video::Path & pathTargeted
				= VTX::MVC_MANAGER().getModel<App::Component::Video::Path>( itemID );
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Path, &pathTargeted, globalClicPos );
		}
		else if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == App::ID::Model::MODEL_VIEWPOINT )
		{
			App::Component::Object3D::Viewpoint & viewpointTargeted
				= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( itemID );
			App::Application::Selection::SelectionModel & selection
				= App::Application::Selection::SelectionManager::get().getSelectionModel();

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
				VTX::UI::ContextualMenu::pop(
					VTX::UI::ContextualMenu::Menu::Viewpoint, &viewpointTargeted, globalClicPos );
			}
		}
	}

	void PathSceneView::_onItemExpanded( QTreeWidgetItem * const p_item )
	{
		_refreshSelection( App::Application::Selection::SelectionManager::get().getSelectionModel() );
		SceneItemWidget::_onItemExpanded( p_item );
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
		if ( p_name.size() > VTX::UI::Style::MOLECULE_NAME_MAXIMUM_SIZE )
			p_name = p_name.substr( 0, VTX::UI::Style::MOLECULE_NAME_MAXIMUM_SIZE );
	}

	QTreeWidgetItem * PathSceneView::_itemFromViewpoint( const App::Component::Object3D::Viewpoint & p_viewpoint ) const
	{
		const App::Core::Model::ID & viewpointID = p_viewpoint.getId();

		for ( int i = 0; i < topLevelItem( 0 )->childCount(); i++ )
		{
			QTreeWidgetItem * const		 item	= topLevelItem( 0 )->child( i );
			const App::Core::Model::ID & itemID = item->data( 0, MODEL_ID_ROLE ).value<App::Core::Model::ID>();
			if ( viewpointID == itemID )
				return item;
		}

		return nullptr;
	}

	void PathSceneView::_addViewpoint( const App::Component::Object3D::Viewpoint * const p_viewpoint )
	{
		QTreeWidgetItem * const viewpointItem = new QTreeWidgetItem();
		viewpointItem->setFlags( viewpointItem->flags() | Qt::ItemFlag::ItemIsEditable );

		viewpointItem->setData(
			0, MODEL_ID_ROLE, QVariant::fromValue<VTX::App::Core::Model::ID>( p_viewpoint->getId() ) );
		viewpointItem->setText( 0, QString::fromStdString( p_viewpoint->getDefaultName() ) );
		viewpointItem->setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( p_viewpoint->getTypeId() ) );

		topLevelItem( 0 )->addChild( viewpointItem );

		_refreshSize();
	}
	void PathSceneView::_removeViewpoint( const App::Component::Object3D::Viewpoint * const p_viewpoint )
	{
		QTreeWidgetItem * const viewpointItem = _itemFromViewpoint( *p_viewpoint );
		if ( viewpointItem != nullptr )
			topLevelItem( 0 )->removeChild( viewpointItem );

		_refreshSize();
	}

} // namespace VTX::View::UI::Widget
