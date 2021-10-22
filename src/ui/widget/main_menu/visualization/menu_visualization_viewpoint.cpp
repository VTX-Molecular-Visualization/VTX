#include "menu_visualization_viewpoint.hpp"
#include "action/action_manager.hpp"
#include "action/viewpoint.hpp"
//#include "mvc/mvc_manager.hpp"
#include "model/path.hpp"
#include "object3d/camera.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationViewpointWidget::MenuVisualizationViewpointWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	};

	void MenuVisualizationViewpointWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			// const bool enableDeleteButton = castedEvent.ptr->hasViewpoint() > 0;
			// _enableDeleteButtonState( enableDeleteButton );
		}
	}

	void MenuVisualizationViewpointWidget::_setupUi( const QString & p_name )
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
	void MenuVisualizationViewpointWidget::_setupSlots()
	{
		_createViewpointButton->setTriggerAction( this, &MenuVisualizationViewpointWidget::_createViewpointAction );
		_deleteViewpointButton->setTriggerAction( this, &MenuVisualizationViewpointWidget::_deleteViewpointAction );
	}

	void MenuVisualizationViewpointWidget::localize() { setTitle( "Selection Action" ); }

	void MenuVisualizationViewpointWidget::_createViewpointAction() const
	{
		const Object3D::Camera &				 cam  = VTXApp::get().getScene().getCamera();
		Model::Path * const						 path = VTXApp::get().getScene().getPaths()[ 0 ];
		Controller::BaseCameraController * const currentController
			= VTXApp::get()
				  .getStateMachine()
				  .getState<State::Visualization>( ID::State::VISUALIZATION )
				  ->getCurrentCameraController();

		VTX_ACTION( new Action::Viewpoint::Create( *path, cam, currentController ) );
	}
	void MenuVisualizationViewpointWidget::_deleteViewpointAction() const {}

	void MenuVisualizationViewpointWidget::_enableDeleteButtonState( const bool p_enable )
	{
		_deleteViewpointButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
