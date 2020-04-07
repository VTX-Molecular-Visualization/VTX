#include "residue.hpp"
#include "action/change_color_mode.hpp"
#include "action/colorable_change_color.hpp "
#include "action/visible_change_visibility.hpp"
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
				ImGui::PushID( ( "ViewResidue" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( ( "Residue: " + _getModel().getSymbolName() ).c_str(), flags ) )
				{
					bool isVisible = _getModel().isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{ VTX_ACTION( new Action::VisibleChangeVisibility( _getModel(), isVisible ) ); }
					ImGui::Text( "ID: %d", _getModel().getIndex() );
					ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
					Vec3f color = _getModel().getColor();
					if ( ImGui::ColorEdit3( "Color", (float *)&color ) )
					{
						VTX_ACTION( new Action::ColorableChangeColor( _getModel(), color ) );
						VTX_ACTION( new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::RESIDUE ) );
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
