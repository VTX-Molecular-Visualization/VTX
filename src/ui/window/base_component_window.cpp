#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			bool BaseComponentWindow::_drawHeader()
			{
				ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
				ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
				ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

				return ImGui::Begin( getTitle(), isVisiblePtr(), flags );
			}

			void BaseComponentWindow::_drawFooter() { ImGui::End(); }
		} // namespace Window
	}	  // namespace UI
} // namespace VTX
