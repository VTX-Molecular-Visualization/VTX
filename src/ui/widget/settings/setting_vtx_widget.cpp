#include "setting_vtx_widget.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "io/struct/image_export.hpp"
#include "setting.hpp"
#include "setting_widget_enum.hpp"
#include "style.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QLabel>

namespace VTX::UI::Widget::Settings
{
	SettingVTXWidget::SettingVTXWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	}

	void SettingVTXWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SETTINGS_CHANGE && !_skipSettingEventsState )
		{
			_refreshData();
		}
	}

	void SettingVTXWidget::_setupUi( const QString & p_name )
	{
		QScrollArea * const settingsScrollArea = new QScrollArea( this );
		QWidget * const		viewport		   = new QWidget( settingsScrollArea );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		_settingsLayout				   = new QGridLayout( viewport );
		_settingsLayout->setContentsMargins( 10, 10, 10, 10 );
		_settingsLayout->setVerticalSpacing( Style::DATA_GRID_VERTICAL_SPACE );
		_settingsLayout->setHorizontalSpacing( Style::DATA_GRID_HORIZONTAL_SPACE );
		_settingsLayout->setColumnStretch( 0, 1 );
		_settingsLayout->setColumnStretch( 1, 10 );

		QHBoxLayout * const bottomLayout = new QHBoxLayout();

		// Controller
		_activeControllerElasticityWidget = new QCheckBox( viewport );
		_controllerElasticityFactorWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerElasticityFactorWidget" );
		_controllerElasticityFactorWidget->setMinMax( 0.f, 1.f );
		_controllerAccelerationFactorWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerAccelerationFactorWidget" );
		_controllerAccelerationFactorWidget->setMinMax( Setting::CONTROLLER_ACCELERATION_FACTOR_MIN,
														Setting::CONTROLLER_ACCELERATION_FACTOR_MAX );
		_controllerDecelerationFactorWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerDecelerationFactorWidget" );
		_controllerDecelerationFactorWidget->setMinMax( Setting::CONTROLLER_DECELERATION_FACTOR_MIN,
														Setting::CONTROLLER_DECELERATION_FACTOR_MAX );
		_controllerTranslationSpeedWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerTranslationSpeedWidget" );
		_controllerTranslationSpeedWidget->setMinMax( Setting::CONTROLLER_TRANSLATION_SPEED_MIN,
													  Setting::CONTROLLER_TRANSLATION_SPEED_MAX );
		_controllerRotationSpeedWidget = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "ControllerRotationSpeedWidget" );
		_controllerRotationSpeedWidget->setMinMax( 0.0f, 1.0f );
		_controllerYAxisInvertedWidget = new QCheckBox( viewport );

		// Camera
		_cameraFOV
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>( viewport, "cameraFov" );
		_cameraFOV->setMinMax( Setting::CAMERA_FOV_MIN, Setting::CAMERA_FOV_MAX );
		_cameraNear
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldDraggableWidget>( viewport, "cameraNear" );
		_cameraNear->setMinMax( Setting::CAMERA_NEAR_MIN, Setting::CAMERA_NEAR_MAX );
		_cameraNear->setLabel( "Near clip" );
		_cameraFar
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldDraggableWidget>( viewport, "cameraNear" );
		_cameraFar->setMinMax( Setting::CAMERA_FAR_MIN, Setting::CAMERA_FAR_MAX );
		_cameraFar->setLabel( "Far clip" );
		_antialiasing	  = new QCheckBox( viewport );
		_cameraProjection = new QComboBox( viewport );
		for ( const std::string & projectionStr : VTXSettings::CAMERA_PROJECTION_STR )
			_cameraProjection->addItem( QString::fromStdString( projectionStr ) );

		// Graphic
		_snapshotFormatWidget = new QComboBox( viewport );
		for ( const std::string & formatStr : IO::Struct::ImageExport::FORMAT_STR )
			_snapshotFormatWidget->addItem( QString::fromStdString( formatStr ) );

		_snapshotBackgroundOpacitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "SnapshotBackgroundOpacitySlider" );
		_snapshotBackgroundOpacitySlider->setMinMax( 0.f, 1.f );

		_snapshotResolutionWidget = new QComboBox( viewport );
		for ( const std::string & resolutionStr : IO::Struct::ImageExport::RESOLUTION_STR )
			_snapshotResolutionWidget->addItem( QString::fromStdString( resolutionStr ) );

		_snapshotQualitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "SnapshotQualitySlider" );
		_snapshotQualitySlider->setMinMax( 0.f, 1.f );

#ifndef VTX_PRODUCTION
		_vsyncWidget		 = new QCheckBox( viewport );
		_forceRendererWidget = new QCheckBox( viewport );
#endif

		_fullscreenWidget = new QCheckBox( viewport );

		// Trajectory
		_defaultTrajectoryPlayModeWidget = new QComboBox( viewport );
		for ( const std::string & playmodeStr : Trajectory::PLAY_MODE_STRING )
			_defaultTrajectoryPlayModeWidget->addItem( QString::fromStdString( playmodeStr ) );

		_defaultTrajectorySpeedWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>(
			viewport, "DefaultTrajectorySpeedWidget" );
		_defaultTrajectorySpeedWidget->setMinMax( Setting::MIN_TRAJECTORY_SPEED, Setting::MAX_TRAJECTORY_SPEED );

		// Data
		_symbolDisplayModeWidget = new QComboBox( viewport );
		for ( const std::string & symbolModeStr : Style::SYMBOL_DISPLAY_MODE_STRING )
			_symbolDisplayModeWidget->addItem( QString::fromStdString( symbolModeStr ) );

		_checkVTXUpdateAtLaunch		= new QCheckBox( viewport );
		_activatePortableSaveWidget = new QCheckBox( viewport );

		_restoreLayoutButton   = new QPushButton( this );
		_restoreSettingsButton = new QPushButton( this );

		_startSection( "Controller" );
		_addItemInLayout( _activeControllerElasticityWidget, "Activate elasticity" );
		_addItemInLayout( _controllerElasticityFactorWidget, "Elasticity strength" );
		_addItemInLayout( _controllerAccelerationFactorWidget, "Acceleration factor" );
		_addItemInLayout( _controllerDecelerationFactorWidget, "Deceleration factor" );
		_addItemInLayout( _controllerTranslationSpeedWidget, "Translation speed" );
		_addItemInLayout( _controllerRotationSpeedWidget, "Rotation speed" );
		_addItemInLayout( _controllerYAxisInvertedWidget, "Invert Y axis" );
		_finishSection();

		_startSection( "Camera" );
		_addItemInLayout( _cameraFOV, "FOV" );
		_addItemInLayout( _cameraNear, _cameraNear->getLabelWidget() );
		_addItemInLayout( _cameraFar, _cameraFar->getLabelWidget() );
		_addItemInLayout( _antialiasing, "Antialiasing" );
		_addItemInLayout( _cameraProjection, "Projection" );
		_finishSection();

		_startSection( "Graphic" );
		_addItemInLayout( _snapshotFormatWidget, "Snapshot format" );
		_addItemInLayout( _snapshotBackgroundOpacitySlider, "Snapshot background opacity" );
		_addItemInLayout( _snapshotResolutionWidget, "Snapshot resolution" );
		_addItemInLayout( _snapshotQualitySlider, "Snapshot quality" );

#ifndef VTX_PRODUCTION
		_addItemInLayout( _vsyncWidget, "VSync" );
		_addItemInLayout( _forceRendererWidget, "Force render" );
#endif
		_addItemInLayout( _fullscreenWidget, "Full screen" );
		_finishSection();

		_startSection( "Trajectory" );
		_addItemInLayout( _defaultTrajectoryPlayModeWidget, "Default play mode" );
		_addItemInLayout( _defaultTrajectorySpeedWidget, "Default play speed" );
		_finishSection();

		_startSection( "Data" );
		_addItemInLayout( _symbolDisplayModeWidget, "Symbol display mode" );
		_addItemInLayout( _activatePortableSaveWidget, "Portable save" );
		_addItemInLayout( _checkVTXUpdateAtLaunch, "Check updates at launch" );

		_addItemInLayout( _restoreLayoutButton );
		_finishSection( false );

		bottomLayout->addStretch( 1000 );
		bottomLayout->addWidget( _restoreSettingsButton );

		mainLayout->addWidget( settingsScrollArea, 1000 );
		mainLayout->addSpacing( 10 );
		mainLayout->addItem( bottomLayout );

		settingsScrollArea->setWidget( viewport );

		_refreshData();
	}

	void SettingVTXWidget::_setupSlots()
	{
		connect( _activeControllerElasticityWidget,
				 &QCheckBox::stateChanged,
				 this,
				 &SettingVTXWidget::_activeControllerElasticityAction );
		connect( _controllerElasticityFactorWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerElasticityFactorAction );
		connect( _controllerAccelerationFactorWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerAccelerationFactorAction );
		connect( _controllerDecelerationFactorWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerDecelerationFactorAction );
		connect( _controllerTranslationSpeedWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerTranslationSpeedAction );
		connect( _controllerRotationSpeedWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerRotationSpeedAction );
		connect( _controllerYAxisInvertedWidget,
				 &QCheckBox::stateChanged,
				 this,
				 &SettingVTXWidget::_changeControllerYInversionAction );

		// Camera
		connect( _cameraFOV,
				 QOverload<float>::of( &CustomWidget::FloatFieldSliderWidget::onValueChange ),
				 this,
				 &SettingVTXWidget::_onCameraFOVChanged );

		connect( _cameraNear,
				 &CustomWidget::FloatFieldDraggableWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_onCameraNearChanged );
		connect( _cameraFar,
				 &CustomWidget::FloatFieldDraggableWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_onCameraFarChanged );
		connect( _antialiasing,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &SettingVTXWidget::_onAntialiasingChanged );
		connect( _cameraProjection,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &SettingVTXWidget::_onCameraProjectionChanged );

		// Graphic
		connect( _snapshotFormatWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeSnapshotFormat );
		connect( _snapshotBackgroundOpacitySlider,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeSnapshotBackgroundOpacity );
		connect( _snapshotResolutionWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeSnapshotResolution );
		connect( _snapshotQualitySlider,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeSnapshotQuality );

#ifndef VTX_PRODUCTION
		connect( _vsyncWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeVSyncAction );
		connect( _forceRendererWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeForceRendererAction );
#endif
		connect( _fullscreenWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeFullscreenAction );

		connect( _defaultTrajectoryPlayModeWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeDefaultTrajectoryPlayMode );
		connect( _defaultTrajectorySpeedWidget,
				 &CustomWidget::IntegerFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeDefaultTrajectorySpeed );

		connect( _symbolDisplayModeWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeSymbolDisplayMode );

		connect(
			_checkVTXUpdateAtLaunch, &QCheckBox::stateChanged, this, &SettingVTXWidget::_changeCheckVTXUpdateAtLaunch );
		connect(
			_activatePortableSaveWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activatePortableSave );

		connect( _restoreLayoutButton, &QPushButton::clicked, this, &SettingVTXWidget::_restoreLayoutAction );

		connect( _restoreSettingsButton, &QPushButton::clicked, this, &SettingVTXWidget::_restoreSettingsAction );
	};
	void SettingVTXWidget::localize()
	{
		_restoreLayoutButton->setText( "Restore layout" );
		_restoreSettingsButton->setText( "Restore" );
	};

	void SettingVTXWidget::_refreshData()
	{
		_activeControllerElasticityWidget->setCheckState(
			Util::UI::getCheckState( VTX_SETTING().getControllerElasticityActive() ) );

		const float elasticityValue
			= 1
			  - ( VTX_SETTING().getControllerElasticityFactor() - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN )
					/ ( Setting::CONTROLLER_ELASTICITY_FACTOR_MAX - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN );
		_controllerElasticityFactorWidget->setValue( elasticityValue );

		_controllerAccelerationFactorWidget->setValue( VTX_SETTING().getAccelerationSpeedFactor() );
		_controllerDecelerationFactorWidget->setValue( VTX_SETTING().getDecelerationSpeedFactor() );
		_controllerTranslationSpeedWidget->setValue( VTX_SETTING().getTranslationSpeed() );

		const float rotationSpeedValue
			= ( VTX_SETTING().getRotationSpeed() - Setting::CONTROLLER_ROTATION_SPEED_MIN )
			  / ( Setting::CONTROLLER_ROTATION_SPEED_MAX - Setting::CONTROLLER_ROTATION_SPEED_MIN );

		_controllerRotationSpeedWidget->setValue( rotationSpeedValue );
		_controllerYAxisInvertedWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getYAxisInverted() ) );

		_cameraFOV->setValue( VTX_SETTING().getCameraFOV() );
		_cameraNear->setValue( VTX_SETTING().getCameraNearClip() );
		_cameraFar->setValue( VTX_SETTING().getCameraFarClip() );
		_antialiasing->setChecked( VTX_SETTING().getAA() );
		const bool							isPerspective = VTX_SETTING().getCameraPerspective();
		const VTXSettings::CameraProjection cameraProjectionEnum
			= isPerspective ? VTXSettings::CameraProjection::PERSPECTIVE : VTXSettings::CameraProjection::ORTHOGRAPHIC;
		_cameraProjection->setCurrentIndex( int( cameraProjectionEnum ) );

		_snapshotFormatWidget->setCurrentIndex( int( VTX_SETTING().getSnapshotFormat() ) );
		_snapshotBackgroundOpacitySlider->setValue( VTX_SETTING().getSnapshotBackgroundOpacity() );
		_snapshotQualitySlider->setValue( VTX_SETTING().getSnapshotQuality() );
		_snapshotResolutionWidget->setCurrentIndex( int( VTX_SETTING().getSnapshotResolution() ) );

#ifndef VTX_PRODUCTION
		_vsyncWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getVSync() ) );
		_forceRendererWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getForceRenderer() ) );
#endif
		_fullscreenWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getWindowFullscreen() ) );

		_defaultTrajectoryPlayModeWidget->setCurrentIndex( int( VTX_SETTING().getDefaultTrajectoryPlayMode() ) );
		_defaultTrajectorySpeedWidget->setValue( VTX_SETTING().getDefaultTrajectorySpeed() );

		_symbolDisplayModeWidget->setCurrentIndex( int( VTX_SETTING().getSymbolDisplayMode() ) );
		_checkVTXUpdateAtLaunch->setCheckState( Util::UI::getCheckState( VTX_SETTING().getCheckVTXUpdateAtLaunch() ) );
		_activatePortableSaveWidget->setCheckState(
			Util::UI::getCheckState( VTX_SETTING().isPortableSaveActivated() ) );
	}

	void SettingVTXWidget::_activeControllerElasticityAction( const bool p_activate )
	{
		_skipSettingEvents();
		if ( VTX_SETTING().getControllerElasticityActive() != p_activate )
			VTX_ACTION( new Action::Setting::ActiveControllerElasticity( p_activate ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerElasticityFactorAction( const float p_value )
	{
		_skipSettingEvents();

		const float elasticityValue
			= ( Setting::CONTROLLER_ELASTICITY_FACTOR_MIN
				+ ( 1 - p_value )
					  * ( Setting::CONTROLLER_ELASTICITY_FACTOR_MAX - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN ) );

		if ( VTX_SETTING().getControllerElasticityFactor() != elasticityValue )
			VTX_ACTION( new Action::Setting::ChangeControllerElasticity( elasticityValue ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerAccelerationFactorAction( const float p_value )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getAccelerationSpeedFactor() != p_value )
			VTX_ACTION( new Action::Setting::ChangeAccelerationFactorSpeed( p_value ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerDecelerationFactorAction( const float p_value )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getDecelerationSpeedFactor() != p_value )
			VTX_ACTION( new Action::Setting::ChangeDecelerationFactorSpeed( p_value ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerTranslationSpeedAction( const float p_value )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getTranslationSpeed() != p_value )
			VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( p_value ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerRotationSpeedAction( const float p_value )
	{
		_skipSettingEvents();

		const float rotationSpeed
			= Setting::CONTROLLER_ROTATION_SPEED_MIN
			  + ( Setting::CONTROLLER_ROTATION_SPEED_MAX - Setting::CONTROLLER_ROTATION_SPEED_MIN ) * p_value;

		if ( VTX_SETTING().getRotationSpeed() != rotationSpeed )
			VTX_ACTION( new Action::Setting::ChangeRotationSpeed( rotationSpeed ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeControllerYInversionAction( const bool p_invert )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getYAxisInverted() != p_invert )
			VTX_ACTION( new Action::Setting::ActiveYAxisInversion( p_invert ) );

		_listenSettingEvents();
	}

	void SettingVTXWidget::_onCameraFOVChanged( const float p_value )
	{
		_skipSettingEvents();

		if ( p_value != VTX_SETTING().getCameraFOV() )
			VTX_ACTION( new Action::Setting::ChangeCameraFov( p_value ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_onCameraNearChanged( const float p_value )
	{
		_skipSettingEvents();

		if ( p_value != VTX_SETTING().getCameraNearClip() )
		{
			const float nearClip = p_value;
			const float farClip	 = VTX_SETTING().getCameraFarClip();

			VTX_ACTION( new Action::Setting::ChangeCameraClip( nearClip, farClip ) );
		}

		_listenSettingEvents();
	}
	void SettingVTXWidget::_onCameraFarChanged( const float p_value )
	{
		_skipSettingEvents();

		if ( p_value != VTX_SETTING().getCameraNearClip() )
		{
			const float nearClip = VTX_SETTING().getCameraNearClip();
			const float farClip	 = p_value;

			VTX_ACTION( new Action::Setting::ChangeCameraClip( nearClip, farClip ) );
		}

		_listenSettingEvents();
	}
	void SettingVTXWidget::_onAntialiasingChanged( const bool p_state )
	{
		_skipSettingEvents();

		if ( p_state != VTX_SETTING().getAA() )
			VTX_ACTION( new Action::Setting::ActiveAA( p_state ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_onCameraProjectionChanged( const int p_value )
	{
		_skipSettingEvents();

		const VTXSettings::CameraProjection projection	  = VTXSettings::CameraProjection( p_value );
		const bool							isPerspective = projection == VTXSettings::CameraProjection::PERSPECTIVE;
		if ( VTX_SETTING().getCameraPerspective() != isPerspective )
			VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( isPerspective ) );

		_listenSettingEvents();
	}

	void SettingVTXWidget::_changeSnapshotFormat( const int p_format )
	{
		_skipSettingEvents();

		const IO::Struct::ImageExport::Format format = IO::Struct::ImageExport::Format( p_format );
		if ( VTX_SETTING().getSnapshotFormat() != format )
			VTX_ACTION( new Action::Setting::ChangeSnapshotFormat( format ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeSnapshotBackgroundOpacity( const float p_opacity )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getSnapshotBackgroundOpacity() != p_opacity )
			VTX_ACTION( new Action::Setting::ChangeBackgroundOpacity( p_opacity ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeSnapshotResolution( const int p_resolution )
	{
		_skipSettingEvents();

		const IO::Struct::ImageExport::RESOLUTION resolution = IO::Struct::ImageExport::RESOLUTION( p_resolution );
		if ( VTX_SETTING().getSnapshotResolution() != resolution )
			VTX_ACTION( new Action::Setting::ChangeSnapshotResolution( resolution ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeSnapshotQuality( const float p_quality )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getSnapshotQuality() != p_quality )
			VTX_ACTION( new Action::Setting::ChangeSnapshotQuality( p_quality ) );

		_listenSettingEvents();
	}

	void SettingVTXWidget::_activeVSyncAction( const bool p_activate )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getVSync() != p_activate )
			VTX_ACTION( new Action::Setting::ActiveVerticalSync( p_activate ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_activeForceRendererAction( const bool p_activate )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getForceRenderer() != p_activate )
			VTX_ACTION( new Action::Setting::ForceRenderer( p_activate ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_activeFullscreenAction( const bool p_activate )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getWindowFullscreen() != p_activate )
		{
			const WindowMode windowMode = p_activate ? WindowMode::Fullscreen : WindowMode::Windowed;
			VTX_ACTION( new Action::Setting::WindowMode( windowMode ) );
		}

		_listenSettingEvents();
	}

	void SettingVTXWidget::_changeDefaultTrajectorySpeed( const int p_fps )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getDefaultTrajectorySpeed() != p_fps )
			VTX_ACTION( new Action::Setting::ChangeDefaultTrajectorySpeed( p_fps ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeDefaultTrajectoryPlayMode( const int p_playmode )
	{
		_skipSettingEvents();

		const Trajectory::PlayMode playMode = Trajectory::PlayMode( p_playmode );
		if ( VTX_SETTING().getDefaultTrajectoryPlayMode() != playMode )
			VTX_ACTION( new Action::Setting::ChangeDefaultTrajectoryPlayMode( playMode ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeSymbolDisplayMode( const int p_displayMode )
	{
		_skipSettingEvents();

		const Style::SYMBOL_DISPLAY_MODE displayMode = Style::SYMBOL_DISPLAY_MODE( p_displayMode );
		if ( VTX_SETTING().getSymbolDisplayMode() != displayMode )
			VTX_ACTION( new Action::Setting::ChangeSymbolDisplayMode( displayMode ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_changeCheckVTXUpdateAtLaunch( const bool p_changeCheckVTXUpdateAtLaunch )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() != p_changeCheckVTXUpdateAtLaunch )
			VTX_ACTION( new Action::Setting::ChangeCheckVTXUpdateAtLaunch( p_changeCheckVTXUpdateAtLaunch ) );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_activatePortableSave( const bool p_activate )
	{
		_skipSettingEvents();

		if ( VTX_SETTING().isPortableSaveActivated() != p_activate )
			VTX_ACTION( new Action::Setting::ActivatePortableSave( p_activate ) );

		_listenSettingEvents();
	}

	void SettingVTXWidget::_restoreLayoutAction()
	{
		_skipSettingEvents();

		VTX_ACTION( new Action::Main::RestoreWindowLayout() );
		VTXApp::get().getMainWindow().showWidget( ID::UI::Window::SETTINGS, true );

		_listenSettingEvents();
	}
	void SettingVTXWidget::_restoreSettingsAction()
	{
		_skipSettingEvents();

		UI::Dialog::confirmActionDialog(
			new Action::Setting::RestoreSetting(), "Confirm", "Are you sure to restore settings ?" );
		// TMP => Move setting into a model
		_refreshData();

		_listenSettingEvents();
	}

	void SettingVTXWidget::_addItemInLayout( QWidget * const p_item, const QString & p_label )
	{
		QLabel * const labelWidget = new QLabel( this );
		labelWidget->setText( p_label );
		_addItemInLayout( p_item, labelWidget );
	}
	void SettingVTXWidget::_addItemInLayout( QWidget * const p_item, QWidget * const p_labelWidget )
	{
		_settingsLayout->addWidget( p_labelWidget, _currentRow, 0 );
		_settingsLayout->addWidget( p_item, _currentRow, 1 );

		_currentRow++;
	}

	void SettingVTXWidget::_addItemInLayout( QWidget * const p_item )
	{
		_settingsLayout->addWidget( p_item, _currentRow, 0, 1, 2, Qt::AlignmentFlag::AlignCenter );
		_currentRow++;
	}
	void SettingVTXWidget::_startSection( const QString & p_title )
	{
		QLabel * const titleWidget = new QLabel( this );
		Util::UI::setDynamicProperty( titleWidget, Style::WidgetProperty::OVERIDDEN_PARAMETER );

		titleWidget->setText( p_title );
		_settingsLayout->addWidget( titleWidget, _currentRow, 0, 1, 2, Qt::AlignmentFlag::AlignLeft );

		_currentRow++;
	}
	void SettingVTXWidget::_finishSection( const bool p_addSpacer )
	{
		if ( p_addSpacer )
		{
			QSpacerItem * const spacer = new QSpacerItem( 0, 30 );
			_settingsLayout->addItem( spacer, _currentRow, 0, 1, 2 );
		}
		_currentRow++;
	}

	void SettingVTXWidget::_skipSettingEvents() { _skipSettingEventsState = true; }
	void SettingVTXWidget::_listenSettingEvents() { _skipSettingEventsState = false; }

} // namespace VTX::UI::Widget::Settings
