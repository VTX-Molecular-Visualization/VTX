#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		bool BaseComponentWindow::_drawHeader()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
			ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

			if ( ImGui::Begin( getTitle(), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return false;
			}

			return true;
		}

		void BaseComponentWindow::_drawContent() { _drawComponents(); }

		void BaseComponentWindow::_drawFooter() { ImGui::End(); }

		void BaseComponentWindow::_draw()
		{
			if ( _drawHeader() == false )
			{
				return;
			}

			_drawContent();
			_drawFooter();
		}

	} // namespace UI
} // namespace VTX
