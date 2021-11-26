#include "scene_widget.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"
#include "view/ui/widget/path_scene_view.hpp"
#include "vtx_app.hpp"
#include <QScrollBar>
#include <algorithm>

namespace VTX::UI::Widget::Scene
{
	SceneWidget::SceneWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::MOLECULE_ADDED );
		_registerEvent( Event::Global::MOLECULE_REMOVED );

		_registerEvent( Event::Global::PATH_ADDED );
		_registerEvent( Event::Global::PATH_REMOVED );
	}

	void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::MOLECULE_ADDED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

			// Set no parent to not trigger ItemChange event during init
			View::UI::Widget::MoleculeSceneView * const moleculeWidget
				= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::MoleculeSceneView>(
					castedEvent.ptr, ID::View::UI_MOLECULE_STRUCTURE, _scrollAreaContent, "moleculeSceneView" );

			_addWidgetInLayout( moleculeWidget );
		}
		else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
			const Model::Molecule * const molecule = castedEvent.ptr;

			View::UI::Widget::MoleculeSceneView * const moleculeWidget
				= MVC::MvcManager::get().getView<View::UI::Widget::MoleculeSceneView>(
					molecule, ID::View::UI_MOLECULE_STRUCTURE );

			_removeWidgetInLayout( moleculeWidget );

			MVC::MvcManager::get().deleteView<View::UI::Widget::MoleculeSceneView>( molecule,
																					ID::View::UI_MOLECULE_STRUCTURE );
		}
		else if ( p_event.name == Event::Global::PATH_ADDED )
		{
			const Event::VTXEventPtr<Model::Path> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Path> &>( p_event );

			// Set no parent to not trigger ItemChange event during init
			View::UI::Widget::PathSceneView * const pathWidget
				= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::PathSceneView>(
					castedEvent.ptr, ID::View::UI_SCENE_PATH, _scrollAreaContent, "pathSceneView" );

			_addWidgetInLayout( pathWidget );
		}
		else if ( p_event.name == Event::Global::PATH_REMOVED )
		{
			const Event::VTXEventPtr<Model::Path> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Path> &>( p_event );
			const Model::Path * const path = castedEvent.ptr;

			View::UI::Widget::PathSceneView * const viewpointWidget
				= MVC::MvcManager::get().getView<View::UI::Widget::PathSceneView>( path, ID::View::UI_SCENE_PATH );

			_removeWidgetInLayout( viewpointWidget );

			MVC::MvcManager::get().deleteView<View::UI::Widget::PathSceneView>( path, ID::View::UI_SCENE_PATH );
		}
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

	int SceneWidget::_getPositionInHierarchy( SceneItemWidget * const p_sceneItemWidget )
	{
		const ID::VTX_ID & modelTypeId = MVC::MvcManager::get().getModelTypeID( p_sceneItemWidget->getModelID() );

		if ( modelTypeId == ID::Model::MODEL_MOLECULE )
		{
			std::vector<SceneItemWidget *>::const_reverse_iterator it	   = _sceneWidgets.crbegin();
			int													   counter = 1;

			for ( it; it != _sceneWidgets.crend(); it++ )
			{
				const ID::VTX_ID & itModelTypeId = MVC::MvcManager::get().getModelTypeID( ( *it )->getModelID() );
				if ( itModelTypeId == modelTypeId )
					return _layout->count() - counter;
				counter++;
			}

			return _layout->count() - counter;
		}

		return _layout->count() - 1;
	}

	void SceneWidget::_addWidgetInLayout( SceneItemWidget * const p_sceneItemWidget )
	{
		const int posInHierarchy = _getPositionInHierarchy( p_sceneItemWidget );
		_layout->insertWidget( posInHierarchy, p_sceneItemWidget, 1 );
		p_sceneItemWidget->updatePosInSceneHierarchy( posInHierarchy );

		if ( _sceneWidgets.size() > 0 )
			setTabOrder( *_sceneWidgets.rbegin(), p_sceneItemWidget );

		_sceneWidgets.insert( _sceneWidgets.begin() + posInHierarchy, p_sceneItemWidget );
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

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>(
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

		for ( const std::pair<Model::Molecule *, float> pairMolecule : VTXApp::get().getScene().getMolecules() )
		{
			View::UI::Widget::MoleculeSceneView * const moleculeWidget
				= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::MoleculeSceneView>(
					pairMolecule.first, ID::View::UI_MOLECULE_STRUCTURE, _scrollAreaContent, "moleculeSceneView" );

			_addWidgetInLayout( moleculeWidget );
		}
		for ( Model::Path * const path : VTXApp::get().getScene().getPaths() )
		{
			View::UI::Widget::PathSceneView * const pathWidget
				= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::PathSceneView>(
					path, ID::View::UI_SCENE_PATH, _scrollAreaContent, "pathSceneView" );

			_addWidgetInLayout( pathWidget );
		}
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
			UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Scene, &scene, p_event->globalPos() );

			p_event->accept();
		}
	}
	void SceneWidget::dragEnterEvent( QDragEnterEvent * p_event )
	{
		BaseManualWidget::dragEnterEvent( p_event );
		if ( p_event->mimeData()->hasFormat(
				 VTX::UI::MimeType::getQStringMimeType( VTX::UI::MimeType::ApplicationMimeType::SCENE_ITEM ) ) )
			p_event->acceptProposedAction();
	}
	void SceneWidget::mouseMoveEvent( QMouseEvent * p_event ) { BaseManualWidget::mouseMoveEvent( p_event ); }

	void SceneWidget::dropEvent( QDropEvent * p_event )
	{
		BaseManualWidget::dropEvent( p_event );

		const QByteArray byteData = p_event->mimeData()->data(
			UI::MimeType::getQStringMimeType( UI::MimeType::ApplicationMimeType::SCENE_ITEM ) );
		const Model::ID idDroppedObject = std::atoi( byteData.data() );

		SceneItemWidget * sceneItemWidget = nullptr;
		int				  previousIndex	  = 0;
		for ( SceneItemWidget * const item : _sceneWidgets )
		{
			if ( item->getModelID() == idDroppedObject )
			{
				sceneItemWidget = item;
				break;
			}
			previousIndex++;
		}

		const int eventPosY				 = _scrollAreaContent->mapFrom( this, p_event->pos() ).y();
		int		  newIndex				 = (int)_sceneWidgets.size() - 1;
		bool	  draggedAfterCurrentPos = false;
		for ( int i = 0; i < _sceneWidgets.size(); i++ )
		{
			const int itemPosY = _sceneWidgets[ i ]->pos().y();
			if ( itemPosY > eventPosY )
			{
				newIndex = i - ( draggedAfterCurrentPos ? 1 : 0 );
				break;
			}

			draggedAfterCurrentPos = draggedAfterCurrentPos || _sceneWidgets[ i ] == sceneItemWidget;
		}

		p_event->acceptProposedAction();

		SceneItemWidget * tmp = _sceneWidgets[ previousIndex ];
		_sceneWidgets.erase( _sceneWidgets.begin() + previousIndex );
		_sceneWidgets.insert( _sceneWidgets.begin() + newIndex, tmp );

		_layout->insertWidget( newIndex, sceneItemWidget, 1 );

		int iMinIndex = previousIndex < newIndex ? previousIndex : newIndex;
		int iMaxIndex = previousIndex > newIndex ? previousIndex : newIndex;
		for ( int i = iMinIndex; i <= iMaxIndex; i++ )
		{
			_sceneWidgets[ i ]->updatePosInSceneHierarchy( i );
		}
	}

} // namespace VTX::UI::Widget::Scene
