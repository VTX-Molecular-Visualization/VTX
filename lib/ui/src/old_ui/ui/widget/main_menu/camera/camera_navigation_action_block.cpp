#include "ui/old_ui/ui/widget/main_menu/camera/camera_navigation_action_block.hpp"
#include "ui/old_ui/state/visualization.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include "ui/qt/action/main.hpp"
#include "ui/qt/action/selection.hpp"
#include <app/action/main.hpp>
#include <app/action/selection.hpp>
#include <app/action/setting.hpp>
#include <app/event/global.hpp>
#include <app/model/selection.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/selection/selection_manager.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	CameraNavigationActionBlock::CameraNavigationActionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::CONTROLLER_CHANGE );
	};

	CameraNavigationActionBlock::~CameraNavigationActionBlock() {}

	void CameraNavigationActionBlock::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::CONTROLLER_CHANGE )
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
		_freefly->setData( "Freecam", ":/sprite/camera_freefly_mode.png", Qt::Orientation::Horizontal );
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
		VTX_ACTION( new App::Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void CameraNavigationActionBlock::_recenterCamera() const
	{
		VTX_ACTION( new QT::Action::Main::ResetCameraController() );
	}
	void CameraNavigationActionBlock::_orientCamera() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new QT::Action::Selection::Orient( selection ) );
	}

	void CameraNavigationActionBlock::_setTrackballController() const
	{
		VTX_ACTION( new QT::Action::Main::ChangeCameraController( ID::Controller::TRACKBALL ) );
	}
	void CameraNavigationActionBlock::_setFreeflyController() const
	{
		VTX_ACTION( new QT::Action::Main::ChangeCameraController( ID::Controller::FREEFLY ) );
	}
	void CameraNavigationActionBlock::_setVesselController() const
	{
		VTX_ACTION( new QT::Action::Main::ChangeCameraController( ID::Controller::VESSEL ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
