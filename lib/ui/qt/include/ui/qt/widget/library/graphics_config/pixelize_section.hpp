#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_PIXELIZE_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_PIXELIZE_SECTION__

#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"
#include <QCheckBox>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class PixelizeSection final : public Section
	{
	  public:
		explicit PixelizeSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<EditableSlider> _sliderSize;
		QPointer<QCheckBox>		 _checkBoxBackground;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
