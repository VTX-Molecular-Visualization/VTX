#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_BACKGROUND_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_BACKGROUND_SECTION__

#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/library/graphics_config/section.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class BackgroundSection final : public Section
	{
	  public:
		explicit BackgroundSection( QWidget * );

		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		QPointer<ColorPicker>	 _colorPicker;
		QPointer<QLineEdit>		 _lineEnvironmentPath;
		QPointer<QPushButton>	 _buttonEnvironmentBrowse;
		QPointer<QPushButton>	 _buttonEnvironmentClear;
		QPointer<QLabel>		 _labelSkyboxIntensity;
		QPointer<EditableSlider> _sliderSkyboxIntensity;
		QPointer<QLabel>		 _labelIblIntensity;
		QPointer<EditableSlider> _sliderIblIntensity;
		QPointer<QLabel>		 _labelEnvironmentRotation;
		QPointer<EditableSlider> _sliderEnvironmentRotation;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
