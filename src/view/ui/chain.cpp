#include "chain.hpp"
#include "action/chain.hpp"
#include "util/ui.hpp"
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
						VTX_ACTION( new Action::Chain::ChangeVisibility(
							_getModel(), Action::Visible::ChangeVisibility::VISIBILITY_MODE::TOGGLE ) );
					}
					ImGui::Text( "ID: %d", _getModel().getIndex() );
					ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
					std::vector<float> color = _getModel().getColor().toStdVector();
					if ( ImGui::ColorEdit3( "Color", (float *)color.data() ) )
					{
						VTX_ACTION( new Action::Chain::ChangeColor( _getModel(), color ) );
					}
					Util::UI::drawRepresentations( _getModel() );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
