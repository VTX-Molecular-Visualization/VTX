#include "menu_visualization_render_effect_button_widget.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"

namespace VTX::UI::Widget::MainMenu
{
	void RenderEffectPresetButton::_setupUi( const QString & p_name )
	{
		MenuToolButtonWidget::_setupUi( p_name );
		setMaximumWidth( Style::MAIN_MENU_MAX_BUTTON_PRESET_WIDTH );
	}

	void RenderEffectPresetButton::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		connect( this, &QToolButton::clicked, this, &RenderEffectPresetButton::_onButtonClicked );
	}

	void RenderEffectPresetButton::_onButtonClicked()
	{
		VTX::Model::Renderer::RenderEffectPreset * renderEffectPreset
			= VTX::Model::Renderer::RenderEffectPresetLibrary::get().getPreset( _id );

		VTX_ACTION( new Action::Renderer::ApplyRenderEffectPreset( *renderEffectPreset, true ) );
	};

} // namespace VTX::UI::Widget::MainMenu
