#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			bool BaseComponentWindow::_drawHeader()
			{
				ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing
										 | ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::SetNextWindowDockID( ImGui::GetID( Style::IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
				ImGui::SetNextWindowBgAlpha( Style::IMGUI_STYLE_BG_ALPHA );

				return ImGui::Begin( getTitle(), isVisiblePtr(), flags );
			}

			void BaseComponentWindow::_drawFooter() { ImGui::End(); }
		} // namespace Window
	}	  // namespace UI
} // namespace VTX
