#include "chain.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Chain::_draw()
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushID( "ViewChain" );
				if ( ImGui::CollapsingHeader( ( "Chain: " + _getModel().getName() ).c_str(), flags ) )
				{
					ImGui::Text( "ID: %d", _getModel().getId() );
					ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
					if ( ImGui::ColorEdit3( "Color", _getModel().getColor() ) )
					{
						Setting::Rendering::colorMode = View::MOLECULE_COLOR_MODE::CHAIN;
						_getModel().getMoleculePtr()->setColorMode();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
