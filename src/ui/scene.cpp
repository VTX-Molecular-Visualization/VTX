#include "scene.hpp"
#include "define.hpp"
#include "localization/language.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		void Scene::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
