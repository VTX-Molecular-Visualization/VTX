#include "view_ui_chain.hpp"
#include "../lib/imgui/imgui.h"

namespace VTX
{
	namespace View
	{
		void ViewUIChain::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIChain::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

			if ( ImGui::CollapsingHeader( ( "Chain: " + _model->getName() ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _model->getId() );
				ImGui::Text( "Residues: %d", _model->getResidueCount() );
			}
		}
	} // namespace View
} // namespace VTX
