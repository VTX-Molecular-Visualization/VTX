#include "atom.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Atom::_draw()
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushID( "ViewAtom" );
				if ( ImGui::CollapsingHeader( ( "Atom: " + _getModel().getSymbolName() ).c_str(), flags ) )
				{
					ImGui::Text( "ID: %d", _getModel().getId() );
					ImGui::Text( "Vdw radius: %.2f", _getModel().getVdwRadius() );
					if ( ImGui::ColorEdit3( "Color", _getModel().getColor() ) )
					{
						Setting::Rendering::colorMode = View::MOLECULE_COLOR_MODE::ATOM;
						_getModel().getMoleculePtr()->setColorMode();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
