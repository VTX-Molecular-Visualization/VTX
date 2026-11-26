#pragma once

#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class ChromaticAberrationSection final : public Section
	{
	  public:
		explicit ChromaticAberrationSection( QWidget * );

		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<EditableSlider> _sliderRed;
		QPointer<EditableSlider> _sliderGreen;
		QPointer<EditableSlider> _sliderBlue;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
