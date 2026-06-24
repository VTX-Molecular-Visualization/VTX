#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SELECTION_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SELECTION_SECTION__

#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class SelectionSection final : public Section
	{
	  public:
		explicit SelectionSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<ColorPicker> _colorPicker;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
