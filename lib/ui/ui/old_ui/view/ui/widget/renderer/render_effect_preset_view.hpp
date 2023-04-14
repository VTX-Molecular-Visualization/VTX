#ifndef __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET__
#define __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET__

#include "old_ui/ui/widget/base_manual_widget.hpp"
#include <app/old_app/model/renderer/render_effect_preset.hpp>
#include <app/old_app/view/base_view.hpp>

namespace VTX::UI::Widget::Settings
{
	class RenderEffectPresetEditor;
}

namespace VTX::View::UI::Widget::Renderer
{
	class RenderEffectPresetView : public View::BaseView<Model::Renderer::RenderEffectPreset>
	{
		VTX_VIEW

		using RenderEffectPresetEditor = VTX::UI::Widget::Settings::RenderEffectPresetEditor;

	  public:
		void setEditor( RenderEffectPresetEditor * const p_linkedEditor ) { _linkedEditor = p_linkedEditor; }

	  protected:
		RenderEffectPresetView( Model::Renderer::RenderEffectPreset * const p_model ) : BaseView( p_model ) {};
		void _refreshView() override;

	  private:
		RenderEffectPresetEditor * _linkedEditor = nullptr;
	};

} // namespace VTX::View::UI::Widget::Renderer

#endif
