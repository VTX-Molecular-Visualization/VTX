#include "render.hpp"
#include "renderer/gl/gl.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		void Render::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
			ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

			if ( ImGui::Begin( LOCALE( "Render.Render" ), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return;
			}

			ImVec2 pos = ImGui::GetCursorScreenPos();

#pragma warning( push )
#pragma warning( disable : 4312 )
			Renderer::GL * r = (Renderer::GL *)&( VTXApp::get().getRenderer() );
			ImGui::GetWindowDrawList()->AddImage(
				(void *)r->getRenderedTexture(),
				ImVec2( ImGui::GetItemRectMin().x + pos.x, ImGui::GetItemRectMin().y + pos.y ),
				ImVec2( pos.x + 800 / 2, pos.y + 600 / 2 ),
				ImVec2( 0, 1 ),
				ImVec2( 1, 0 ) );
#pragma warning( pop )
			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
