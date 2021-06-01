#ifndef __VTX_UI_WIDGET_SETTINGS_VTX__
#define __VTX_UI_WIDGET_SETTINGS_VTX__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/integer_field_widget.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingVTXWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingVTXWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshData();

	  private:
		QGridLayout * _settingsLayout		 = nullptr;
		QPushButton * _restoreSettingsButton = nullptr;

		// Controller
		QCheckBox *							   _activeControllerElasticityWidget  = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerElasticityFactorWidget  = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerTranslationFactorWidget = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerTranslationSpeedWidget  = nullptr;
		CustomWidget::FloatFieldSliderWidget * _controllerRotationSpeedWidget	  = nullptr;
		QCheckBox *							   _controllerYAxisInvertedWidget	  = nullptr;

		// Graphic
		CustomWidget::FloatFieldSliderWidget * _snapshotBackgroundOpacitySlider = nullptr;
		QCheckBox *							   _vsyncWidget						= nullptr;
		QCheckBox *							   _forceRendererWidget				= nullptr;
		QCheckBox *							   _fullscreenWidget				= nullptr;

		// Trajectory
		QComboBox *						   _defaultTrajectoryPlayModeWidget = nullptr;
		CustomWidget::IntegerFieldWidget * _defaultTrajectorySpeedWidget	= nullptr;

		// Data
		QComboBox *	  _symbolDisplayModeWidget = nullptr;
		QPushButton * _restoreLayoutButton	   = nullptr;

		int _currentRow = 0;

		void _activeControllerElasticityAction( const bool p_activate );
		void _changeControllerElasticityFactorAction( const float p_value );
		void _changeControllerTranslationFactorAction( const float p_value );
		void _changeControllerTranslationSpeedAction( const float p_value );
		void _changeControllerRotationSpeedAction( const float p_value );
		void _changeControllerYInversionAction( const bool p_invert );

		void _changeSnapshotBackgroundOpacity( const float p_opacity );
		void _activeVSyncAction( const bool p_activate );
		void _activeForceRendererAction( const bool p_activate );
		void _activeFullscreenAction( const bool p_activate );

		void _changeDefaultTrajectorySpeed( const int p_fps );
		void _changeDefaultTrajectoryPlayMode( const int p_playmode );

		void _changeSymbolDisplayMode( const int p_displayMode );

		void _restoreSettingsAction();
		void _restoreLayoutAction();

		void _addItemInLayout( QWidget * const p_item, const QString & p_label );
		void _addItemInLayout( QWidget * const p_item );

		void _startSection( const QString & p_title );
		void _finishSection();
	};
} // namespace VTX::UI::Widget::Settings
#endif
