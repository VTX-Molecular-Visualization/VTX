#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_OUTLINE_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_OUTLINE_SECTION__

#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class OutlineSection final : public Section
	{
	  public:
		explicit OutlineSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<ColorPicker>	 _colorPicker;
		QPointer<EditableSlider> _sliderSensitivity;
		QPointer<EditableSlider> _sliderThickness;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
