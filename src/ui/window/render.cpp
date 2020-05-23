#include "render.hpp"
#include "action/resize.hpp"
#include "renderer/gl/gl.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			bool Render::_drawHeader()
			{
				ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
				ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
				ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

				return ImGui::Begin( getTitle(), isVisiblePtr(), flags );
			}

			void Render::_drawContent()
			{
				ImVec2 position = ImGui::GetWindowPos();			  // Window position.
				ImVec2 size		= ImGui::GetWindowSize();			  // Window size with bar.
				ImVec2 min		= ImGui::GetWindowContentRegionMin(); // Content region min (window without bar/border).
				ImVec2 max		= ImGui::GetWindowContentRegionMax(); // Content region max.

				uint width	= uint( max.x - min.x );
				uint height = uint( max.y - min.y );

				if ( width != _width || height != _height )
				{
					_width	= width;
					_height = height;
					VTX_ACTION( new Action::Resize( _width, _height ) );
				}

#pragma warning( push )
#pragma warning( disable : 4312 )
				const Renderer::GL & renderer = VTXApp::get().getRendererGL();
				ImGui::GetWindowDrawList()->AddImage( (void *)( renderer.getRenderedTexture() ),
													  ImVec2( position.x + min.x, position.y + min.y ),
													  ImVec2( position.x + min.x + width, position.y + min.y + height ),
													  ImVec2( 0, 1 ),
													  ImVec2( 1, 0 ) );
#pragma warning( pop )
			}
		} // namespace Window
	}	  // namespace UI
} // namespace VTX
