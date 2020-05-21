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

			ImVec2 position = ImGui::GetWindowPos();
			ImVec2 min		= ImGui::GetWindowContentRegionMin();
			ImVec2 max		= ImGui::GetWindowContentRegionMax();

			min.x += position.x;
			min.y += position.y;
			max.x += position.x;
			max.y += position.y;

#pragma warning( push )
#pragma warning( disable : 4312 )
			Renderer::GL * r = (Renderer::GL *)&( VTXApp::get().getRenderer() );
			ImGui::GetWindowDrawList()->AddImage(
				(void *)r->getRenderedTexture(), min, max, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
#pragma warning( pop )
			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
