#include "camera_editor.hpp"
#include "localization/language.hpp"
#include "style.hpp"

namespace VTX
{
	namespace UI
	{
		void CameraEditor::_draw()
		{
			ImGuiWindowFlags flagsWindow = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
			ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

			if ( ImGui::Begin( LOCALE( "CameraEditor.CameraEditor" ), isVisiblePtr(), flagsWindow ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}
	} // namespace UI

} // namespace VTX
