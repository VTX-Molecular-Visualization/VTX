#include "residue.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Residue::_draw()
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushID( "ViewResidue" );
				if ( ImGui::CollapsingHeader( ( "Residue: " + _getModel().getSymbolName() ).c_str(), flags ) )
				{
					ImGui::Text( "ID: %d", _getModel().getId() );
					ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
					if ( ImGui::ColorEdit3( "Color", _getModel().getColor() ) )
					{
						Setting::Rendering::colorMode = View::MOLECULE_COLOR_MODE::RESIDUE;
						_getModel().getMoleculePtr()->setColorMode();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
