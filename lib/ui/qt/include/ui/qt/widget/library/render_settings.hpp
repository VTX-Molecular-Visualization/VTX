#ifndef __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__
#define __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__

#include "base_preset_widget.hpp"
#include "ui/qt/core/widget/editable_slider.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include <QLabel>
#include <app/action/render_settings.hpp>
#include <app/library/preset/render_settings.hpp>

namespace VTX::UI::QT::Widget::Library
{
	class RenderSettings final : public BasePresetWidget<App::Library::Preset::RenderSettings>
	{
	  public:
		RenderSettings( QWidget * p_parent );

	  protected:
		void _onPresetAdded( const std::string_view ) override;
		void _onPresetChanged() override;

	  private:
		// Shading.
		QPointer<Core::Widget::HideableGroupBox> _groupboxShading;
		QPointer<QComboBox>						 _comboBoxShadingMode;
		QPointer<Widget::ColorPicker>			 _colorPickerBackground;
		QPointer<Widget::ColorPicker>			 _colorPickerLight;
		QPointer<QLabel>						 _labelSpecularFactor;
		QPointer<Core::Widget::EditableSlider>	 _sliderSpecularFactor;
		QPointer<QLabel>						 _labelShininess;
		QPointer<Core::Widget::EditableSlider>	 _sliderShininess;
		QPointer<QLabel>						 _labelToonSteps;
		QPointer<Core::Widget::EditableSlider>	 _sliderToonSteps;

		// SSAO.
		QPointer<Core::Widget::HideableGroupBox> _groupboxSSAO;
		QPointer<Core::Widget::EditableSlider>	 _sliderSSAOIntensity;
		QPointer<Core::Widget::EditableSlider>	 _sliderBlurSize;

		// Outline.
		QPointer<Core::Widget::HideableGroupBox> _groupboxOutline;
		QPointer<Widget::ColorPicker>			 _colorPickerOutline;
		QPointer<Core::Widget::EditableSlider>	 _sliderOutlineSensitivity;
		QPointer<Core::Widget::EditableSlider>	 _sliderOutlineThickness;

		// Fog.
		QPointer<Core::Widget::HideableGroupBox> _groupboxFog;
		QPointer<Widget::ColorPicker>			 _colorPickerFog;
		QPointer<Core::Widget::EditableSlider>	 _sliderFogNear;
		QPointer<Core::Widget::EditableSlider>	 _sliderFogFar;
		QPointer<Core::Widget::EditableSlider>	 _sliderFogDensity;

		// Selection.
		QPointer<Core::Widget::HideableGroupBox> _groupboxSelection;
		QPointer<Widget::ColorPicker>			 _colorPickerSelection;

		template<VTX::Renderer::E_RENDER_SETTINGS S, typename T>
		void _changeValue( const T p_value )
		{
			App::ACTION_SYSTEM().execute<App::Action::RenderSettings::Change<S, T>>( _preset, p_value );
		}

		void _applyLogic();
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
