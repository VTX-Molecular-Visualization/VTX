#include "component_selection.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentSelection::ComponentSelection( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentSelection::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Selection.Selection" ), _show, flags ) == false )
			{
				ImGui::End();
				return;
			}

			if ( _model == nullptr )
			{
				ImGui::End();
				return;
			}

			ImGui::End();
		}

		void ComponentSelection::notify( Event::EVENT_MODEL p_event ) {}
	} // namespace UI
} // namespace VTX
