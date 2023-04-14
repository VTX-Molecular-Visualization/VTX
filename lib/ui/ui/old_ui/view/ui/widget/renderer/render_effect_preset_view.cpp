#include "render_effect_preset_view.hpp"
#include "old_ui/ui/widget/settings/render_effect_preset_editor.hpp"

namespace VTX::View::UI::Widget::Renderer
{
	void RenderEffectPresetView::_refreshView() { _linkedEditor->refresh(); };
} // namespace VTX::View::UI::Widget::Renderer
