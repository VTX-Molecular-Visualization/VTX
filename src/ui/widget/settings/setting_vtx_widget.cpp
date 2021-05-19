#include "setting_vtx_widget.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "setting.hpp"
#include "style.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QLabel>

namespace VTX::UI::Widget::Settings
{
	SettingVTXWidget::SettingVTXWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent ) {}

	void SettingVTXWidget::receiveEvent( const Event::VTXEvent & p_event ) {}

	void SettingVTXWidget::_setupUi( const QString & p_name )
	{
		QScrollArea * const settingsScrollArea = new QScrollArea( this );
		QWidget * const		viewport		   = new QWidget( settingsScrollArea );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		_settingsLayout				   = new QGridLayout( viewport );
		_settingsLayout->setMargin( 10 );
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
		_controllerTranslationFactorWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerTranslationFactorWidget" );
		_controllerTranslationFactorWidget->setMinMax( Setting::CONTROLLER_TRANSLATION_FACTOR_MIN,
													   Setting::CONTROLLER_TRANSLATION_FACTOR_MAX );
		_controllerTranslationSpeedWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				viewport, "ControllerTranslationSpeedWidget" );
		_controllerTranslationSpeedWidget->setMinMax( Setting::CONTROLLER_TRANSLATION_SPEED_MIN,
													  Setting::CONTROLLER_TRANSLATION_SPEED_MAX );
		_controllerYAxisInvertedWidget = new QCheckBox( viewport );

		// Graphic
		_snapshotBackgroundOpacitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "SnapshotBackgroundOpacitySlider" );
		_snapshotBackgroundOpacitySlider->setMinMax( 0.f, 1.f );

		_vsyncWidget		 = new QCheckBox( viewport );
		_forceRendererWidget = new QCheckBox( viewport );
		_fullscreenWidget	 = new QCheckBox( viewport );

		// Trajectory
		_defaultTrajectoryPlayModeWidget = new QComboBox( viewport );
		for ( const std::string & playmodeStr : Trajectory::PLAY_MODE_STRING )
			_defaultTrajectoryPlayModeWidget->addItem( QString::fromStdString( playmodeStr ) );

		_defaultTrajectorySpeedWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldWidget>(
			viewport, "DefaultTrajectorySpeedWidget" );
		_defaultTrajectorySpeedWidget->setMinMax( Setting::MIN_TRAJECTORY_SPEED, Setting::MAX_TRAJECTORY_SPEED );

		// Data
		_symbolDisplayModeWidget = new QComboBox( viewport );
		for ( const std::string & symbolModeStr : Style::SYMBOL_DISPLAY_MODE_STRING )
			_symbolDisplayModeWidget->addItem( QString::fromStdString( symbolModeStr ) );

		_restoreSettingsButton = new QPushButton( this );
		_restoreSettingsButton->setText( "Restore" );

		_startSection( "Controller" );
		_addItemInLayout( _activeControllerElasticityWidget, "Activate elasticity" );
		_addItemInLayout( _controllerElasticityFactorWidget, "Elasticity strength" );
		_addItemInLayout( _controllerTranslationFactorWidget, "Translation factor" );
		_addItemInLayout( _controllerTranslationSpeedWidget, "Translation speed" );
		_addItemInLayout( _controllerYAxisInvertedWidget, "Invert Y axis" );
		_finishSection();

		_startSection( "Graphic" );
		_addItemInLayout( _snapshotBackgroundOpacitySlider, "Snapshot background opacity" );
		_addItemInLayout( _vsyncWidget, "VSync" );
		_addItemInLayout( _forceRendererWidget, "Force render" );
		_addItemInLayout( _fullscreenWidget, "Full screen" );
		_finishSection();

		_startSection( "Trajectory" );
		_addItemInLayout( _defaultTrajectoryPlayModeWidget, "Default play mode" );
		_addItemInLayout( _defaultTrajectorySpeedWidget, "Default play speed" );
		_finishSection();

		_startSection( "Data" );
		_addItemInLayout( _symbolDisplayModeWidget, "Symbol display mode" );
		_finishSection();

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
		connect( _controllerTranslationFactorWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerTranslationFactorAction );
		connect( _controllerTranslationSpeedWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeControllerTranslationSpeedAction );
		connect( _controllerYAxisInvertedWidget,
				 &QCheckBox::stateChanged,
				 this,
				 &SettingVTXWidget::_changeControllerYInversionAction );

		connect( _snapshotBackgroundOpacitySlider,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeSnapshotBackgroundOpacity );
		connect( _vsyncWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeVSyncAction );
		connect( _forceRendererWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeForceRendererAction );
		connect( _fullscreenWidget, &QCheckBox::stateChanged, this, &SettingVTXWidget::_activeFullscreenAction );

		connect( _defaultTrajectoryPlayModeWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeDefaultTrajectoryPlayMode );
		connect( _defaultTrajectorySpeedWidget,
				 &CustomWidget::IntegerFieldWidget::onValueChange,
				 this,
				 &SettingVTXWidget::_changeDefaultTrajectorySpeed );

		connect( _symbolDisplayModeWidget,
				 QOverload<int>::of( ( &QComboBox::currentIndexChanged ) ),
				 this,
				 &SettingVTXWidget::_changeSymbolDisplayMode );

		connect( _restoreSettingsButton, &QPushButton::clicked, this, &SettingVTXWidget::_restoreSettingsAction );
	};
	void SettingVTXWidget::localize() {};

	void SettingVTXWidget::_refreshData()
	{
		_activeControllerElasticityWidget->setCheckState(
			Util::UI::getCheckState( VTX_SETTING().getControllerElasticityActive() ) );

		const float elasticityValue
			= 1
			  - ( VTX_SETTING().getControllerElasticityFactor() - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN )
					/ ( Setting::CONTROLLER_ELASTICITY_FACTOR_MAX - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN );
		_controllerElasticityFactorWidget->setValue( elasticityValue );

		_controllerTranslationFactorWidget->setValue( VTX_SETTING().getTranslationSpeedFactor() );
		_controllerTranslationSpeedWidget->setValue( VTX_SETTING().getTranslationSpeed() );
		_controllerYAxisInvertedWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getYAxisInverted() ) );

		_snapshotBackgroundOpacitySlider->setValue( VTX_SETTING().getSnapshotBackgroundOpacity() );
		_vsyncWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getVSync() ) );
		_forceRendererWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getForceRenderer() ) );
		_fullscreenWidget->setCheckState( Util::UI::getCheckState( VTX_SETTING().getWindowFullscreen() ) );

		_defaultTrajectoryPlayModeWidget->setCurrentIndex( int( VTX_SETTING().getDefaultTrajectoryPlayMode() ) );
		_defaultTrajectorySpeedWidget->setValue( VTX_SETTING().getDefaultTrajectorySpeed() );

		_symbolDisplayModeWidget->setCurrentIndex( int( VTX_SETTING().getSymbolDisplayMode() ) );
	}

	void SettingVTXWidget::_activeControllerElasticityAction( const bool p_activate )
	{
		if ( VTX_SETTING().getControllerElasticityActive() != p_activate )
			VTX_ACTION( new Action::Setting::ActiveControllerElasticity( p_activate ) );
	}
	void SettingVTXWidget::_changeControllerElasticityFactorAction( const float p_value )
	{
		const float elasticityValue
			= ( Setting::CONTROLLER_ELASTICITY_FACTOR_MIN
				+ ( 1 - p_value )
					  * ( Setting::CONTROLLER_ELASTICITY_FACTOR_MAX - Setting::CONTROLLER_ELASTICITY_FACTOR_MIN ) );

		if ( VTX_SETTING().getControllerElasticityFactor() != elasticityValue )
			VTX_ACTION( new Action::Setting::ChangeControllerElasticity( elasticityValue ) );
	}
	void SettingVTXWidget::_changeControllerTranslationFactorAction( const float p_value )
	{
		if ( VTX_SETTING().getTranslationSpeedFactor() != p_value )
			VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed( p_value ) );
	}
	void SettingVTXWidget::_changeControllerTranslationSpeedAction( const float p_value )
	{
		if ( VTX_SETTING().getTranslationSpeed() != p_value )
			VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( p_value ) );
	}
	void SettingVTXWidget::_changeControllerYInversionAction( const bool p_invert )
	{
		if ( VTX_SETTING().getYAxisInverted() != p_invert )
			VTX_ACTION( new Action::Setting::ActiveYAxisInversion( p_invert ) );
	}

	void SettingVTXWidget::_changeSnapshotBackgroundOpacity( const float p_opacity )
	{
		if ( VTX_SETTING().getSnapshotBackgroundOpacity() != p_opacity )
			VTX_ACTION( new Action::Setting::ChangeBackgroundOpacity( p_opacity ) );
	}
	void SettingVTXWidget::_activeVSyncAction( const bool p_activate )
	{
		if ( VTX_SETTING().getVSync() != p_activate )
			VTX_ACTION( new Action::Setting::ActiveVerticalSync( p_activate ) );
	}
	void SettingVTXWidget::_activeForceRendererAction( const bool p_activate )
	{
		if ( VTX_SETTING().getForceRenderer() != p_activate )
			VTX_ACTION( new Action::Setting::ForceRenderer( p_activate ) );
	}
	void SettingVTXWidget::_activeFullscreenAction( const bool p_activate )
	{
		if ( VTX_SETTING().getWindowFullscreen() != p_activate )
		{
			const WindowMode windowMode = p_activate ? WindowMode::Fullscreen : WindowMode::Windowed;
			VTX_ACTION( new Action::Setting::WindowMode( windowMode ) );
		}
	}

	void SettingVTXWidget::_changeDefaultTrajectorySpeed( const int p_fps )
	{
		if ( VTX_SETTING().getDefaultTrajectorySpeed() != p_fps )
			VTX_ACTION( new Action::Setting::ChangeDefaultTrajectorySpeed( p_fps ) );
	}
	void SettingVTXWidget::_changeDefaultTrajectoryPlayMode( const int p_playmode )
	{
		const Trajectory::PlayMode playMode = Trajectory::PlayMode( p_playmode );
		if ( VTX_SETTING().getDefaultTrajectoryPlayMode() != playMode )
			VTX_ACTION( new Action::Setting::ChangeDefaultTrajectoryPlayMode( playMode ) );
	}
	void SettingVTXWidget::_changeSymbolDisplayMode( const int p_displayMode )
	{
		const Style::SYMBOL_DISPLAY_MODE displayMode = Style::SYMBOL_DISPLAY_MODE( p_displayMode );
		if ( VTX_SETTING().getSymbolDisplayMode() != displayMode )
			VTX_ACTION( new Action::Setting::ChangeSymbolDisplayMode( displayMode ) );
	}

	void SettingVTXWidget::_restoreSettingsAction()
	{
		UI::Dialog::confirmActionDialog(
			new Action::Setting::RestoreSetting(), "Confirm", "Are you sure to restore settings ?" );
		// TMP => Move setting into a model
		_refreshData();
	}

	void SettingVTXWidget::_addItemInLayout( QWidget * const p_item, const QString & p_label )
	{
		QLabel * const labelWidget = new QLabel( this );
		labelWidget->setText( p_label );
		_settingsLayout->addWidget( labelWidget, _currentRow, 0 );
		_settingsLayout->addWidget( p_item, _currentRow, 1 );

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
	void SettingVTXWidget::_finishSection()
	{
		QSpacerItem * const spacer = new QSpacerItem( 0, 30 );
		_settingsLayout->addItem( spacer, _currentRow, 0, 1, 2 );

		_currentRow++;
	}

} // namespace VTX::UI::Widget::Settings
