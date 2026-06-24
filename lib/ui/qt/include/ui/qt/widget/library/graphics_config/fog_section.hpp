#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_FOG_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_FOG_SECTION__

#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class FogSection final : public Section
	{
	  public:
		explicit FogSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<ColorPicker>	 _colorPicker;
		QPointer<EditableSlider> _sliderNear;
		QPointer<EditableSlider> _sliderFar;
		QPointer<EditableSlider> _sliderDensity;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
