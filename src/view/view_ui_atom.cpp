#include "view_ui_atom.hpp"
#include "../lib/imgui/imgui.h"

namespace VTX
{
	namespace View
	{
		void ViewUIAtom::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIAtom::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewAtom" );
			if ( ImGui::CollapsingHeader( ( "Atom: " + _model->getName() ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _model->getId() );
				ImGui::Text( "Vdw radius: %.2f", _model->getVdwRadius() );
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
