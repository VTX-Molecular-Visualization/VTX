#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SSAO_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SSAO_SECTION__

#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"
#include <QComboBox>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class SSAOSection final : public Section
	{
	  public:
		explicit SSAOSection( QWidget * );
		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<QComboBox>		 _comboBoxMethod;
		QPointer<QComboBox>		 _comboBoxScale;
		QPointer<EditableSlider> _sliderIntensity;
		QPointer<EditableSlider> _sliderBlurSize;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
