#ifndef __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__
#define __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__

#include "base_preset_widget.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/hideable_goupe_box.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <app/action/graphics_config.hpp>

namespace VTX::UI::QT::Widget::Library
{
	class GraphicsConfig final : public BasePresetWidget<VTX::Renderer::GraphicsConfig>
	{
	  public:
		GraphicsConfig( QWidget * p_parent );

	  protected:
		void _update( Entity ) override;

	  private:
		// Background.
		QPointer<HideableGroupBox> _groupboxBackground;

		// Shading.
		QPointer<HideableGroupBox> _groupboxShading;
		QPointer<QComboBox>		   _comboBoxShadingMode;
		QPointer<ColorPicker>	   _colorPickerBackground;
		QPointer<ColorPicker>	   _colorPickerLight;
		QPointer<QLabel>		   _labelSpecularFactor;
		QPointer<EditableSlider>   _sliderSpecularFactor;
		QPointer<QLabel>		   _labelShininess;
		QPointer<EditableSlider>   _sliderShininess;
		QPointer<QLabel>		   _labelToonSteps;
		QPointer<EditableSlider>   _sliderToonSteps;
		QPointer<QLabel>		   _labelMaterial;
		QPointer<QListWidget>	   _listMaterials;
		QPointer<QLineEdit>		   _lineEnvironmentPath;
		QPointer<QPushButton>	   _buttonEnvironmentBrowse;
		QPointer<QPushButton>	   _buttonEnvironmentClear;

		// SSAO.
		QPointer<HideableGroupBox> _groupboxSSAO;
		QPointer<QComboBox>		   _comboBoxSSAOMethod;
		QPointer<QComboBox>		   _comboBoxSSAOScale;
		QPointer<EditableSlider>   _sliderSSAOIntensity;
		QPointer<EditableSlider>   _sliderBlurSize;

		// Outline.
		QPointer<HideableGroupBox> _groupboxOutline;
		QPointer<ColorPicker>	   _colorPickerOutline;
		QPointer<EditableSlider>   _sliderOutlineSensitivity;
		QPointer<EditableSlider>   _sliderOutlineThickness;

		// Fog.
		QPointer<HideableGroupBox> _groupboxFog;
		QPointer<ColorPicker>	   _colorPickerFog;
		QPointer<EditableSlider>   _sliderFogNear;
		QPointer<EditableSlider>   _sliderFogFar;
		QPointer<EditableSlider>   _sliderFogDensity;

		// Chromatic aberration.
		QPointer<HideableGroupBox> _groupboxChromaticAberration;
		QPointer<EditableSlider>   _sliderChromaticAberrationRed;
		QPointer<EditableSlider>   _sliderChromaticAberrationGreen;
		QPointer<EditableSlider>   _sliderChromaticAberrationBlue;

		// Pixelize.
		QPointer<HideableGroupBox> _groupboxPixelize;
		QPointer<EditableSlider>   _sliderPixelizeSize;
		QPointer<QCheckBox>		   _checkBoxPixelizeBackground;

		// CRT.
		QPointer<HideableGroupBox> _groupboxCRT;
		QPointer<EditableSlider>   _sliderCRTCurvatureX;
		QPointer<EditableSlider>   _sliderCRTCurvatureY;
		QPointer<EditableSlider>   _sliderCRTRatio;
		QPointer<EditableSlider>   _sliderCRTGraninessX;
		QPointer<EditableSlider>   _sliderCRTGraninessY;
		QPointer<EditableSlider>   _sliderCRTVignetteRoundness;
		QPointer<EditableSlider>   _sliderCRTVignetteIntensity;
		QPointer<EditableSlider>   _sliderCRTBrightness;

		// Selection.
		QPointer<HideableGroupBox> _groupboxSelection;
		QPointer<ColorPicker>	   _colorPickerSelection;

		template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
		void _changeValue( const T p_value )
		{ App::ACTION().execute<App::Action::GraphicsConfig::Change<S, T>>( currentPreset(), p_value ); }

		void _applyLogic( const VTX::Renderer::GraphicsConfig & );
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
