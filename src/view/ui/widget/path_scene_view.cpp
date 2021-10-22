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
#include "ui/widget/scene/molecule_selection_model.hpp"
#include "ui/widget/scene/scene_widget.hpp"
#include "ui/widget_factory.hpp"
#include "util/string.hpp"
#include <QScrollBar>

namespace VTX::View::UI::Widget
{
	PathSceneView::PathSceneView( Model::Path * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Path>( p_model ), SceneItemWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
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
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			_refreshSelection( *castedEvent.ptr );
		}
		else if ( p_event.name == Event::Global::VIEWPOINT_ADDED )
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

	void PathSceneView::keyPressEvent( QKeyEvent * p_event )
	{
		if ( p_event->key() == Qt::Key::Key_F2 )
		{
			const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

			// TODO
			// Override rename key binding because multiple selection with molecule make it fail
			// if ( selection.isViewpointSelected( *_model ) && selection.getItems().size() == 1 )
			//{
			//	openRenameEditor();
			//}
		}
		else if ( p_event->key() == Qt::Key::Key_Up )
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
		else if ( p_event->key() == Qt::Key::Key_Down )
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
			SceneItemWidget::keyPressEvent( p_event );
		}
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
		connect( this, &QTreeWidget::customContextMenuRequested, this, &PathSceneView::_onCustomContextMenuCalled );
	}
	void PathSceneView::localize() { SceneItemWidget::localize(); }

	void PathSceneView::_createTopLevelObject()
	{
		SceneItemWidget::_createTopLevelObject();
		topLevelItem( 0 )->setText( 0, QString::fromStdString( Style::VIEWPOINT_GROUP_NAME ) );
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
		// TODO

		/*VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const QPoint globalClicPos = mapToGlobal( p_clicPos );

		Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();

		if ( selection.isViewpointSelected( *_model ) )
		{
			VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
				= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
					VTX::UI::ContextualMenu::Menu::Selection );
			selectionContextualMenu->setFocusedTarget( _model );
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
		}
		else
		{
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Viewpoint, _model, globalClicPos );
		}*/
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
	void PathSceneView::_refreshSelection( const Model::Selection & p_selection )
	{
		// const Model::Selection::MapMoleculeIds &			   items = p_selection.getItems();
		// const Model::Selection::MapMoleculeIds::const_iterator itMoleculeItem
		//	= p_selection.getItems().find( _model->getId() );

		//_enableSignals( false );
		// QItemSelection selection = QItemSelection();

		// if ( itMoleculeItem != p_selection.getItems().end() )
		//{
		//	QTreeWidgetItem * const moleculeItem = _getMoleculeTreeWidgetItem();
		//	selection.append( QItemSelectionRange( indexFromItem( moleculeItem ) ) );

		//	if ( moleculeItem->isExpanded() )
		//	{
		//		const Model::Chain * topChain			  = nullptr;
		//		const Model::Chain * previousChain		  = nullptr;
		//		QModelIndex			 topChainItemIndex	  = QModelIndex();
		//		QModelIndex			 bottomChainItemIndex = QModelIndex();

		//		for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : itMoleculeItem->second )
		//		{
		//			const Model::Chain & chain = *_model->getChain( pairChain.first );

		//			QTreeWidgetItem * const chainItem = moleculeItem->child( chain.getIndex() );

		//			if ( topChainItemIndex.isValid() )
		//			{
		//				// if not contiguous, add new range
		//				if ( chain.getIndex() != uint( previousChain->getIndex() + 1 ) )
		//				{
		//					selection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
		//					topChainItemIndex = indexFromItem( chainItem );
		//					topChain		  = &chain;
		//				}

		//				bottomChainItemIndex = indexFromItem( chainItem );
		//			}
		//			else
		//			{
		//				topChainItemIndex	 = indexFromItem( chainItem );
		//				topChain			 = &chain;
		//				bottomChainItemIndex = indexFromItem( chainItem );
		//			}

		//			previousChain = &chain;

		//			if ( !chainItem->isExpanded() )
		//				continue;

		//			const Model::Residue * topResidue			  = nullptr;
		//			const Model::Residue * previousResidue		  = nullptr;
		//			QModelIndex			   topResidueItemIndex	  = QModelIndex();
		//			QModelIndex			   bottomResidueItemIndex = QModelIndex();
		//			for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
		//			{
		//				const Model::Residue &	residue = *_model->getResidue( pairResidue.first );
		//				QTreeWidgetItem * const residueItem
		//					= chainItem->child( residue.getIndex() - chain.getIndexFirstResidue() );

		//				if ( topResidueItemIndex.isValid() )
		//				{
		//					// if not contiguous, add new range
		//					if ( residue.getIndex() != uint( previousResidue->getIndex() + 1 ) )
		//					{
		//						selection.append( QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex )
		//); 						topResidueItemIndex = indexFromItem( residueItem ); topResidue = &residue;
		//					}

		//					bottomResidueItemIndex = indexFromItem( residueItem );
		//				}
		//				else
		//				{
		//					topResidueItemIndex	   = indexFromItem( residueItem );
		//					topResidue			   = &residue;
		//					bottomResidueItemIndex = indexFromItem( residueItem );
		//				}

		//				previousResidue = &residue;

		//				if ( !residueItem->isExpanded() )
		//					continue;

		//				const Model::Atom * topAtom				= nullptr;
		//				const Model::Atom * previousAtom		= nullptr;
		//				QModelIndex			topAtomItemIndex	= QModelIndex();
		//				QModelIndex			bottomAtomItemIndex = QModelIndex();
		//				for ( const uint & atomId : pairResidue.second )
		//				{
		//					const Model::Atom &		atom = *_model->getAtom( atomId );
		//					QTreeWidgetItem * const atomItem
		//						= residueItem->child( atomId - residue.getIndexFirstAtom() );

		//					if ( topAtomItemIndex.isValid() )
		//					{
		//						// if not contiguous, add new range
		//						if ( atom.getIndex() != uint( previousAtom->getIndex() + 1 ) )
		//						{
		//							selection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex )
		//); 							topAtom			 = &atom; 							topAtomItemIndex =
		// indexFromItem( atomItem
		//);
		//						}

		//						bottomAtomItemIndex = indexFromItem( atomItem );
		//					}
		//					else
		//					{
		//						topAtomItemIndex	= indexFromItem( atomItem );
		//						topAtom				= &atom;
		//						bottomAtomItemIndex = indexFromItem( atomItem );
		//					}

		//					previousAtom = &atom;
		//				}
		//				if ( topAtomItemIndex.isValid() )
		//				{
		//					selection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
		//				}
		//			}

		//			if ( topResidueItemIndex.isValid() )
		//			{
		//				selection.append( QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
		//			}
		//		}

		//		if ( topChainItemIndex.isValid() )
		//		{
		//			selection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
		//		}
		//	}
		//}

		// VTX::UI::Widget::Scene::MoleculeSelectionModel * const selectModel
		//	= dynamic_cast<VTX::UI::Widget::Scene::MoleculeSelectionModel * const>( selectionModel() );

		// selectModel->refreshSelection( selection );

		//_enableSignals( true );
	}

	QMimeData * PathSceneView::_getDataForDrag() { return VTX::UI::MimeType::generatePathData( *_model ); }

	void PathSceneView::openRenameEditor() { editItem( topLevelItem( 0 ) ); }
	void PathSceneView::updatePosInSceneHierarchy( const int p_position )
	{
		SceneItemWidget::updatePosInSceneHierarchy( p_position );

		// TODO Manage item scene position
		//_model->getConfiguration().sceneIndex = p_position;
	}

	QTreeWidgetItem * PathSceneView::itemFromViewpoint( const Model::Viewpoint & p_viewpoint ) const
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
		QTreeWidgetItem * const viewpointItem = itemFromViewpoint( *p_viewpoint );
		if ( viewpointItem != nullptr )
			topLevelItem( 0 )->removeChild( viewpointItem );

		_refreshSize();
	}

} // namespace VTX::View::UI::Widget
