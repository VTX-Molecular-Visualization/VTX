#ifndef __VTX_STYLE__
#define __VTX_STYLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <imgui/imgui.h>

namespace VTX
{
	constexpr char * IMGUI_ID_MAIN_WINDOW	 = "VTXMainWindow";
	constexpr char * IMGUI_ID_MAIN_DOCKSPACE = "VTXMainDockSpace";

	constexpr bool IMGUI_SHOW_DEMO		  = false;
	constexpr bool IMGUI_ENABLE_VIEWPORTS = false;

	constexpr float IMGUI_STYLE_ROUNDING	   = 0.f;
	constexpr float IMGUI_STYLE_WINDOW_BORDER  = 0.f;
	constexpr float IMGUI_STYLE_WINDOW_PADDING = 0.f;
	constexpr float IMGUI_STYLE_BG_ALPHA	   = 1.f;

	constexpr float IMGUI_STYLE_MENUBAR_PADDING = 8.f;

	constexpr float IMGUI_STYLE_PROGRESS_BG_ALPHA  = 0.5f;
	static ImColor	IMGUI_STYLE_PROGRESS_BAR_COLOR = ImColor( 150, 150, 150 );
	static ImVec2	IMGUI_STYLE_PROGRESS_BAR_SIZE  = ImVec2( 600, 70 );

} // namespace VTX

#endif
