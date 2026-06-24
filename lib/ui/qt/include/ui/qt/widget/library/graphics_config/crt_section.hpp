#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_CRT_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_CRT_SECTION__

#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class CRTSection final : public Section
	{
	  public:
		explicit CRTSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<EditableSlider> _sliderCurvatureX;
		QPointer<EditableSlider> _sliderCurvatureY;
		QPointer<EditableSlider> _sliderRatio;
		QPointer<EditableSlider> _sliderGraninessX;
		QPointer<EditableSlider> _sliderGraninessY;
		QPointer<EditableSlider> _sliderVignetteRoundness;
		QPointer<EditableSlider> _sliderVignetteIntensity;
		QPointer<EditableSlider> _sliderBrightness;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
