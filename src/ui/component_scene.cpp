#include "component_scene.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../view/view_ui_molecule.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentScene::_addComponents()
		{
			_addComponent( COMPONENT_TYPE::VIEW_MOLECULE,
						   std::make_shared<View::ViewUIMolecule>( View::ViewUIMolecule( _show ) ) );
		}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flagsWindow = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _show, flagsWindow ) == false )
			{
				ImGui::End();
				return;
			}

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			if ( ImGui::CollapsingHeader( LOCALE( "Scene.Performance" ), flags ) )
			{
				ImGuiIO & io = ImGui::GetIO();

				ImGui::Text( "Time UI:  %.0f ms", VTXApp::get().getTimeLastUI() * 1000.0 );
				ImGui::Text( "Time renderer:  %.0f ms", VTXApp::get().getTimeLastRenderer() * 1000.0 );
				ImGui::Text( "FPS ImGui: %.0f", io.Framerate );

				try
				{
					ImGui::Text( "FPS computed: %.0f", 1.f / VTXApp::get().getTimeLast() );
				}
				catch ( const std::exception )
				{
				}
			}

			_drawComponent( COMPONENT_TYPE::VIEW_MOLECULE );

			ImGui::End();
		}

	} // namespace UI
} // namespace VTX
