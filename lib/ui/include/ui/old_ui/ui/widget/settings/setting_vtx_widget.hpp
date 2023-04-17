#ifndef __VTX_UI_WIDGET_SETTINGS_VTX__
#define __VTX_UI_WIDGET_SETTINGS_VTX__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/float_field_draggable_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/integer_field_slider_widget.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
#include <app/old_app/io/struct/image_export.hpp>

namespace VTX::UI::Widget::Settings
{
	class SettingVTXWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingVTXWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshData();

		void _skipSettingEvents();
		void _listenSettingEvents();

	  private:
		QPushButton * _restoreSettingsButton = nullptr;

		// Controller
		QCheckBox *							   _activeControllerElasticityWidget   = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerElasticityFactorWidget   = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerAccelerationFactorWidget = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerDecelerationFactorWidget = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerTranslationSpeedWidget   = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerRotationSpeedWidget	   = nullptr;
		QCheckBox *							   _controllerYAxisInvertedWidget	   = nullptr;

		// Camera
		CustomWidget::FloatFieldSliderWidget *	  _cameraFOV		= nullptr;
		CustomWidget::FloatFieldDraggableWidget * _cameraNear		= nullptr;
		CustomWidget::FloatFieldDraggableWidget * _cameraFar		= nullptr;
		QCheckBox *								  _antialiasing		= nullptr;
		QComboBox *								  _cameraProjection = nullptr;

		// Graphic
		QComboBox *							   _snapshotFormatWidget			= nullptr;
		CustomWidget::FloatFieldSliderWidget * _snapshotBackgroundOpacitySlider = nullptr;
		QComboBox *							   _snapshotResolutionWidget		= nullptr;
		CustomWidget::FloatFieldSliderWidget * _snapshotQualitySlider			= nullptr;
		QCheckBox *							   _vsyncWidget						= nullptr;
		QCheckBox *							   _forceRendererWidget				= nullptr;
		QCheckBox *							   _fullscreenWidget				= nullptr;

		// Trajectory
		QComboBox *								 _defaultTrajectoryPlayModeWidget = nullptr;
		CustomWidget::IntegerFieldSliderWidget * _defaultTrajectorySpeedWidget	  = nullptr;

		// Data
		QComboBox *	  _symbolDisplayModeWidget	  = nullptr;
		QCheckBox *	  _activatePortableSaveWidget = nullptr;
		QCheckBox *	  _checkVTXUpdateAtLaunch	  = nullptr;
		QPushButton * _restoreLayoutButton		  = nullptr;

		int	 _currentRow			 = 0;
		bool _skipSettingEventsState = false;

		void _activeControllerElasticityAction( const bool p_activate );
		void _changeControllerElasticityFactorAction( const float p_value );
		void _changeControllerAccelerationFactorAction( const float p_value );
		void _changeControllerDecelerationFactorAction( const float p_value );
		void _changeControllerTranslationSpeedAction( const float p_value );
		void _changeControllerRotationSpeedAction( const float p_value );
		void _changeControllerYInversionAction( const bool p_invert );

		void _onCameraFOVChanged( const float p_value );
		void _onCameraNearChanged( const float p_value );
		void _onCameraFarChanged( const float p_value );
		void _onAntialiasingChanged( const bool p_state );
		void _onCameraProjectionChanged( const int p_state );

		void _changeSnapshotFormat( const int p_format );
		void _changeSnapshotBackgroundOpacity( const float p_opacity );
		void _changeSnapshotResolution( const int p_resolution );
		void _changeSnapshotQuality( const float p_quality );
		void _activeVSyncAction( const bool p_activate );
		void _activeForceRendererAction( const bool p_activate );
		void _activeFullscreenAction( const bool p_activate );

		void _changeDefaultTrajectorySpeed( const int p_fps );
		void _changeDefaultTrajectoryPlayMode( const int p_playmode );

		void _changeSymbolDisplayMode( const int p_displayMode );
		void _changeCheckVTXUpdateAtLaunch( const bool p_changeCheckVTXUpdateAtLaunch );
		void _activatePortableSave( const bool p_activate );

		void _restoreSettingsAction();
		void _restoreLayoutAction();
	};
} // namespace VTX::UI::Widget::Settings
#endif
