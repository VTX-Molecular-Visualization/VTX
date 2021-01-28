#include "menu_visualization_render_effect_button_widget.hpp"
#include "action/action_manager.hpp"
//#include "action/renderer.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"

namespace VTX::UI::Widget::MainMenu
{
	void RenderEffectPresetButton::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		connect( this, &QToolButton::clicked, this, &RenderEffectPresetButton::_onButtonClicked );
	}

	void RenderEffectPresetButton::_onButtonClicked()
	{
		VTX::Model::Renderer::RenderEffectPreset * renderEffectPreset
			= VTX::Model::Renderer::RenderEffectPresetLibrary::get().getPreset( _id );

		renderEffectPreset->apply();
		// TODO : do action (circular dependences ??)
		// VTX_ACTION( new Action::Renderer::ApplyRenderEffectPreset( *renderEffectPreset ) );
	};

} // namespace VTX::UI::Widget::MainMenu
