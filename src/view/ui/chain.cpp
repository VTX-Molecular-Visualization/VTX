#include "chain.hpp"
#include "action/chain_change_visibility.hpp"
#include "action/change_color_mode.hpp"
#include "action/colorable_change_color.hpp"
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
				ImGui::PushID( ( "ViewChain" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( ( "Chain: " + _getModel().getName() ).c_str(), flags ) )
				{
					bool isVisible = _getModel().isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::ChainChangeVisibility(
							_getModel(), Action::VisibleChangeVisibility::VISIBILITY_MODE::TOGGLE ) );
					}
					ImGui::Text( "ID: %d", _getModel().getIndex() );
					ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
					Color::Rgb color = _getModel().getColor();
					if ( ImGui::ColorEdit3( "Color", (float *)&color ) )
					{
						VTX_ACTION( new Action::ColorableChangeColor( _getModel(), color ) );
						VTX_ACTION( new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::CHAIN ) );
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
