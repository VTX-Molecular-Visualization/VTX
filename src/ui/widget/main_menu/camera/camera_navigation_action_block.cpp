#include "camera_navigation_action_block.hpp"
#include "action/main.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include "state/visualization.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	CameraNavigationActionBlock::CameraNavigationActionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::CONTROLLER_CHANGE );
	};

	CameraNavigationActionBlock::~CameraNavigationActionBlock() {}

	void CameraNavigationActionBlock::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::CONTROLLER_CHANGE )
		{
			_updateCameraModeFeedback();
		}
	}

	void CameraNavigationActionBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Selection focus
		_center = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_center->setData( "Reset", ":/sprite/camera_recenter_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_center, 0 );

		_reorient
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_reorient->setData( "Orient", ":/sprite/camera_orient_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_reorient, 0 );

		// Camera Mode
		_trackball
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_trackball->setData( "Trackball", ":/sprite/camera_trackball_mode.png", Qt::Orientation::Horizontal );
		pushButton( *_trackball, 1 );

		_freefly
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_freefly->setData( "Freefly", ":/sprite/camera_freefly_mode.png", Qt::Orientation::Horizontal );
		pushButton( *_freefly, 1 );

		// !V0.1
		//_vessel = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton"
		//); _vessel->setData( "Vessel", ":/sprite/camera_vessel_mode.png", Qt::Orientation::Horizontal ); pushButton(
		// *_vessel, 2 );

		// !V0.1
		// Viewpoints
		//_createViewpoint
		//	= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "createViewpointButton" );
		//_createViewpoint->setData( "Add\nViewpoint", ":/sprite/add_viewpoint.png", Qt::Orientation::Horizontal );
		// pushButton( *_createViewpoint, 3 );

		validate();

		_updateCameraModeFeedback();
	}

	void CameraNavigationActionBlock::localize() { setTitle( "Navigation" ); }

	void CameraNavigationActionBlock::_setupSlots()
	{
		_center->setTriggerAction( this, &CameraNavigationActionBlock::_recenterCamera );
		_reorient->setTriggerAction( this, &CameraNavigationActionBlock::_orientCamera );

		_trackball->setTriggerAction( this, &CameraNavigationActionBlock::_setTrackballController );
		_freefly->setTriggerAction( this, &CameraNavigationActionBlock::_setFreeflyController );
		// !V0.1
		// _vessel->setTriggerAction( this, &CameraNavigationActionBlock::_setVesselController );
	}

	void CameraNavigationActionBlock::_updateCameraModeFeedback()
	{
		const ID::VTX_ID currentControllerID = VTXApp::get()
												   .getStateMachine()
												   .getState<State::Visualization>( ID::State::VISUALIZATION )
												   ->getCurrentCameraControllerID();

		_trackball->showActiveFeedback( currentControllerID == ID::Controller::TRACKBALL );
		_freefly->showActiveFeedback( currentControllerID == ID::Controller::FREEFLY );
		// !V0.1
		// _vessel->showActiveFeedback( currentControllerID == ID::Controller::VESSEL );
	}

	void CameraNavigationActionBlock::_toggleCameraProjection() const
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void CameraNavigationActionBlock::_recenterCamera() const
	{
		VTX_ACTION( new Action::Main::ResetCameraController() );
	}
	void CameraNavigationActionBlock::_orientCamera() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Orient( selection ) );
	}

	void CameraNavigationActionBlock::_setTrackballController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::TRACKBALL ) );
	}
	void CameraNavigationActionBlock::_setFreeflyController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::FREEFLY ) );
	}
	void CameraNavigationActionBlock::_setVesselController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::VESSEL ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
