#include "component_console.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"

namespace VTX
{
	namespace UI
	{
		void ComponentConsole::_draw()
		{
			ImGui::SetNextWindowSize( ImVec2( 520, 600 ),
									  ImGuiCond_FirstUseEver );
			if ( !ImGui::Begin( "Console", NULL ) )
			{
				ImGui::End();
				return;
			}

			ImGui::End();
		}

		void ComponentConsole::_clear() {}
	} // namespace UI
} // namespace VTX
