#include "component_scene.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentScene::ComponentScene( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentScene::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Scene.Scene" ), _show, flags ) == false )
			{
				ImGui::End();
				return;
			}

			// ImGui::ListBoxHeader( "" );
			// ImGui::ListBoxFooter();

			ImGui::End();
		}

		void ComponentScene::notify( Event::EVENT_MODEL p_event )
		{
			if ( p_event == Event::EVENT_MODEL::UPDATE ) { VTX_INFO( "NOTIFY VIEW GUI" ); }
		}
	} // namespace UI
} // namespace VTX
