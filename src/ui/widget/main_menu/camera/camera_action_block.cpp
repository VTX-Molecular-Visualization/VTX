#include "camera_action_block.hpp"
#include "action/main.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include "state/visualization.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	CameraActionBlock::CameraActionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::CONTROLLER_CHANGE );
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	};

	CameraActionBlock::~CameraActionBlock() {}

	void CameraActionBlock::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::CONTROLLER_CHANGE )
		{
			_updateCameraModeFeedback();
		}
		else if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.end() )
			{
				_refreshCameraProjectionButton();
			}
		}
	}

	void CameraActionBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Selection focus
		_cameraProjectionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_cameraProjectionButton->setData( "", "", Qt::Orientation::Vertical );
		pushButton( *_cameraProjectionButton, 0 );

		// Selection focus
		_center = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_center->setData( "Reset", ":/sprite/camera_recenter_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_center, 1 );

		_reorient
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_reorient->setData( "Orient", ":/sprite/camera_orient_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_reorient, 1 );

		// Camera Mode
		_trackball
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_trackball->setData( "Trackball", ":/sprite/camera_trackball_mode.png", Qt::Orientation::Horizontal );
		pushButton( *_trackball, 2 );

		_freefly
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
		_freefly->setData( "Freecam", ":/sprite/camera_freefly_mode.png", Qt::Orientation::Horizontal );
		pushButton( *_freefly, 2 );

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
		_refreshCameraProjectionButton();
	}

	void CameraActionBlock::localize() { setTitle( "Camera Action" ); }

	void CameraActionBlock::_setupSlots()
	{
		_cameraProjectionButton->setTriggerAction( this, &CameraActionBlock::_toggleCameraProjection );

		_center->setTriggerAction( this, &CameraActionBlock::_recenterCamera );
		_reorient->setTriggerAction( this, &CameraActionBlock::_orientCamera );

		_trackball->setTriggerAction( this, &CameraActionBlock::_setTrackballController );
		_freefly->setTriggerAction( this, &CameraActionBlock::_setFreeflyController );
		// !V0.1
		// _vessel->setTriggerAction( this, &CameraActionBlock::_setVesselController );
	}

	void CameraActionBlock::_updateCameraModeFeedback()
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

	void CameraActionBlock::_refreshCameraProjectionButton() const
	{
		const bool											  isPerspective = VTX_SETTING().getCameraPerspective();
		const Widget::Settings::VTXSettings::CameraProjection settingCameraProj
			= isPerspective ? Widget::Settings::VTXSettings::CameraProjection::PERSPECTIVE
							: Widget::Settings::VTXSettings::CameraProjection::ORTHOGRAPHIC;

		const QString text = QString::fromStdString(
			Widget::Settings::VTXSettings::CAMERA_PROJECTION_STR[ int( settingCameraProj ) ] );

		const QString iconPath = isPerspective ? ":/sprite/camera_projection_perspective_icon.png"
											   : ":/sprite/camera_projection_ortho_icon.png";

		_cameraProjectionButton->setText( text );
		_cameraProjectionButton->setIcon( QIcon::fromTheme( iconPath ) );
	}

	void CameraActionBlock::_toggleCameraProjection() const
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void CameraActionBlock::_recenterCamera() const { VTX_ACTION( new Action::Main::ResetCameraController() ); }
	void CameraActionBlock::_orientCamera() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Orient( selection ) );
	}

	void CameraActionBlock::_setTrackballController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::TRACKBALL ) );
	}
	void CameraActionBlock::_setFreeflyController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::FREEFLY ) );
	}
	void CameraActionBlock::_setVesselController() const
	{
		VTX_ACTION( new Action::Main::ChangeCameraController( ID::Controller::VESSEL ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
