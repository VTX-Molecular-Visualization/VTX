#include "component_scene.hpp"
#include "define.hpp"
#include "localization/language.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _visible, flags ) == false )
			{
				ImGui::End();
				return;
			}

			_drawComponents();

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
