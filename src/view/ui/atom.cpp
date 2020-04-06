#include "atom.hpp"
#include "action/change_color_mode.hpp"
#include "action/colorable_change_color.hpp"
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
				ImGui::PushID( ( "ViewAtom" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( ( "Atom: " + _getModel().getSymbolName() ).c_str(), flags ) )
				{
					ImGui::Text( "ID: %d", _getModel().getIndex() );
					ImGui::Text( "Vdw radius: %.2f", _getModel().getVdwRadius() );
					/*
					Vec3f color = _getModel().getColor();
					if ( ImGui::ColorEdit3( "Color", (float *)&color ) )
					{
						VTX_ACTION(
							new Action::ColorableChangeColor( _getModel(), color ) );
						VTX_ACTION(
							new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::ATOM ) );
					}
					*/
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
