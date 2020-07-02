#include "residue.hpp"
#include "action/residue.hpp"
#include "util/ui.hpp"
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
					/*
					bool isVisible = _getModel().isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::VisibleChangeVisibility(
							_getModel(), (Action::VisibleChangeVisibility::VISIBILITY_MODE)isVisible ) );
					}
					*/
					ImGui::Text( "ID: %d", _getModel().getIndex() );
					ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
					std::vector<float> color = _getModel().getColor().toStdVector();
					if ( ImGui::ColorEdit3( "Color", (float *)color.data() ) )
					{
						VTX_ACTION( new Action::Residue::ChangeColor( _getModel(), color ) );
						// VTX_ACTION( new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::RESIDUE ) );
					}
					Util::UI::drawRepresentations( _getModel() );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
