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
				ImGui::PushID( ( "ViewChain" + std::to_string( _model->getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( ( "Chain: " + _model->getName() ).c_str(), flags ) )
				{
					bool isVisible = _model->isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::Chain::ChangeVisibility(
							*_model, Action::Visible::ChangeVisibility::VISIBILITY_MODE::TOGGLE ) );
					}
					ImGui::Text( "ID: %d", _model->getIndex() );
					ImGui::Text( "Residues: %d", _model->getResidueCount() );
					Color::Rgb color = _model->getColor();
					if ( ImGui::ColorEdit3( "Color", (float *)&color ) )
					{
						VTX_ACTION( new Action::Chain::ChangeColor( *_model, color ) );
					}
					Util::UI::drawRepresentations( *_model, *_model->getMoleculePtr() );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
