#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_WIDGET__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_WIDGET__

#include "base_preset_widget.hpp"
#include "graphics_config/background_section.hpp"
#include "graphics_config/chromatic_aberration_section.hpp"
#include "graphics_config/crt_section.hpp"
#include "graphics_config/fog_section.hpp"
#include "graphics_config/lighting_section.hpp"
#include "graphics_config/outline_section.hpp"
#include "graphics_config/pixelize_section.hpp"
#include "graphics_config/selection_section.hpp"
#include "graphics_config/ssao_section.hpp"

namespace VTX::UI::QT::Widget::Library
{
	class GraphicsConfigWidget final : public BasePresetWidget<VTX::Renderer::GraphicsConfig>
	{
	  public:
		explicit GraphicsConfigWidget( QWidget * );

	  protected:
		void _update( Entity ) override;

	  private:
		QPointer<GraphicsConfig::BackgroundSection>			 _backgroundSection;
		QPointer<GraphicsConfig::LightingSection>			 _lightingSection;
		QPointer<GraphicsConfig::SSAOSection>				 _ssaoSection;
		QPointer<GraphicsConfig::SelectionSection>			 _selectionSection;
		QPointer<GraphicsConfig::OutlineSection>			 _outlineSection;
		QPointer<GraphicsConfig::FogSection>				 _fogSection;
		QPointer<GraphicsConfig::ChromaticAberrationSection> _chromaticAberrationSection;
		QPointer<GraphicsConfig::PixelizeSection>			 _pixelizeSection;
		QPointer<GraphicsConfig::CRTSection>				 _crtSection;
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
