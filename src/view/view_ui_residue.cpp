#include "view_ui_residue.hpp"
#include "../lib/imgui/imgui.h"

namespace VTX
{
	namespace View
	{
		void ViewUIResidue::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIResidue::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

			if ( ImGui::CollapsingHeader( ( "Residue: " + _model->getSymbolName() ).c_str() ), flags )
			{
				ImGui::Text( "ID: %d", _model->getId() );
				ImGui::Text( "Atoms: %d", _model->getAtomCount() );
			}
		}
	} // namespace View
} // namespace VTX
