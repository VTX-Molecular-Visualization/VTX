#include "viewpoint_block.hpp"
#include "action/action_manager.hpp"
#include "action/viewpoint.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	ViewpointBlock::ViewpointBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	};

	void ViewpointBlock::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			const Model::Selection * const selectionModel = castedEvent.ptr;
			_enableDeleteButtonState( selectionModel->hasItemOfType( VTX::ID::Model::MODEL_VIEWPOINT ) );
		}
	}

	void ViewpointBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_createViewpointButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "createViewpointButton" );
		_createViewpointButton->setData( "Create", ":/sprite/add_viewpoint.png", Qt::Orientation::Horizontal );
		pushButton( *_createViewpointButton, 0 );

		_deleteViewpointButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "deleteViewpointButton" );
		_deleteViewpointButton->setData( "Delete", ":/sprite/delete_viewpoint.png", Qt::Orientation::Horizontal );
		pushButton( *_deleteViewpointButton, 0 );

		_enableDeleteButtonState( false );

		validate();
	}
	void ViewpointBlock::_setupSlots()
	{
		_createViewpointButton->setTriggerAction( this, &ViewpointBlock::_createViewpointAction );
		_deleteViewpointButton->setTriggerAction( this, &ViewpointBlock::_deleteViewpointAction );
	}

	void ViewpointBlock::localize() { setTitle( "Viewpoints" ); }

	void ViewpointBlock::_createViewpointAction() const { VTX_ACTION( new Action::Viewpoint::Create() ); }
	void ViewpointBlock::_deleteViewpointAction() const
	{
		std::vector<Model::Viewpoint *> viewpointsInSelection = std::vector<Model::Viewpoint *>();
		const Model::Selection &		selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		viewpointsInSelection.reserve( selection.getItems().size() );

		for ( const Model::ID & modelID : selection.getItems() )
		{
			const ID::VTX_ID & modelTypeID = MVC::MvcManager::get().getModelTypeID( modelID );
			if ( modelTypeID == VTX::ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & viewpoint = MVC::MvcManager::get().getModel<Model::Viewpoint>( modelID );
				viewpointsInSelection.emplace_back( &viewpoint );
			}
		}

		VTX_ACTION( new Action::Viewpoint::Delete( viewpointsInSelection ) );
	}

	void ViewpointBlock::_enableDeleteButtonState( const bool p_enable )
	{
		_deleteViewpointButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
