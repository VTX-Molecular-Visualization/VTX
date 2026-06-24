#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_LIGHTING_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_LIGHTING_SECTION__

#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/material_section.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"
#include <QComboBox>
#include <QLabel>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class LightingSection final : public Section
	{
	  public:
		explicit LightingSection( QWidget * );

		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<QComboBox>		  _comboBoxShadingMode;
		QPointer<ColorPicker>	  _colorPickerLight;
		QPointer<QLabel>		  _labelLightIntensity;
		QPointer<EditableSlider>  _sliderLightIntensity;
		QPointer<QLabel>		  _labelAmbientIntensity;
		QPointer<EditableSlider>  _sliderAmbientIntensity;
		QPointer<QLabel>		  _labelToneMappingMode;
		QPointer<QComboBox>		  _comboBoxToneMappingMode;
		QPointer<QLabel>		  _labelToneMappingExposure;
		QPointer<EditableSlider>  _sliderToneMappingExposure;
		QPointer<QLabel>		  _labelSpecularFactor;
		QPointer<EditableSlider>  _sliderSpecularFactor;
		QPointer<QLabel>		  _labelShininess;
		QPointer<EditableSlider>  _sliderShininess;
		QPointer<QLabel>		  _labelToonSteps;
		QPointer<EditableSlider>  _sliderToonSteps;
		QPointer<MaterialSection> _materialSection;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
