#ifndef __VTX_UI_QT_TOOL_KEYS__
#define __VTX_UI_QT_TOOL_KEYS__

#include "__new_archi/ui/core/define.hpp"
#include "__new_archi/ui/core/layout_descriptor.hpp"

namespace VTX::UI::QT::Tool
{
	inline static const VTX::UI::Core::ToolIdentifier RENDER_WINDOW_KEY = "VTX_RenderWindow";
	inline static const VTX::UI::Core::ToolIdentifier SCENE_WINDOW_KEY	= "VTX_SceneWindow";
	inline static const VTX::UI::Core::ToolIdentifier SESSION_TOOLS_KEY = "VTX_SessionTools";

	namespace ContextualMenu
	{
		const Core::WidgetKey RENDER	= "VTX_ContextualMenu_Render";
		const Core::WidgetKey SCENE		= "VTX_ContextualMenu_Scene";
		const Core::WidgetKey SELECTION = "VTX_ContextualMenu_Selection";
	}; // namespace ContextualMenu

} // namespace VTX::UI::QT::Tool

#endif
