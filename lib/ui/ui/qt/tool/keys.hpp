#ifndef __VTX_UI_QT_TOOL_KEYS__
#define __VTX_UI_QT_TOOL_KEYS__

#include "core/define.hpp"
#include "core/layout_descriptor.hpp"

namespace VTX::UI::QT::Tool
{
	inline static const VTX::UI::Core::ToolIdentifier RENDER_WINDOW_KEY		   = "VTX_RenderWindow";
	inline static const VTX::UI::Core::ToolIdentifier SCENE_WINDOW_KEY		   = "VTX_SceneWindow";
	inline static const VTX::UI::Core::ToolIdentifier SESSION_TOOLS_KEY		   = "VTX_SessionTools";
	inline static const VTX::UI::Core::ToolIdentifier UI_FEATURE_INFORMATION   = "VTX_UIFeatureInformation";
	inline static const VTX::UI::Core::ToolIdentifier UI_FEATURE_PANEL_DISPLAY = "VTX_UIFeaturePanelDisplay";
	inline static const VTX::UI::Core::ToolIdentifier UI_FEATURE_QUIT		   = "VTX_UIFeatureQuit";
	inline static const VTX::UI::Core::ToolIdentifier CHECK_UPDATE_KEY		   = "VTX_CheckUpdateKey";

	namespace ContextualMenu
	{
		const Core::WidgetKey RENDER	= "VTX_ContextualMenu_Render";
		const Core::WidgetKey SCENE		= "VTX_ContextualMenu_Scene";
		const Core::WidgetKey SELECTION = "VTX_ContextualMenu_Selection";
	}; // namespace ContextualMenu

	namespace Cursor
	{
		const Core::WidgetKey DEFAULT = "VTX_Cursor_Default";
	}; // namespace Cursor

} // namespace VTX::UI::QT::Tool

#endif
