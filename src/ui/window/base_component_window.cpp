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

			void BaseComponentWindow::_drawContent() { _drawComponents(); }

			void BaseComponentWindow::_draw()
			{
				if ( _drawHeader() == false )
				{
					ImGui::End();
					return;
				}

				_drawContent();
				ImGui::End();
			}
		} // namespace Window
	}	  // namespace UI
} // namespace VTX
