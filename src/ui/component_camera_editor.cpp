#include "component_camera_editor.hpp"
#include "localization/language.hpp"
#include "style.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentCameraEditor::ComponentCameraEditor( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentCameraEditor::_draw()
		{
			ImGuiWindowFlags flagsWindow = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "CameraEditor.CameraEditor" ), _visible, flagsWindow ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}
	} // namespace UI

} // namespace VTX
