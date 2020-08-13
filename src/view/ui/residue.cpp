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
				ImGui::PushID( ( "ViewResidue" + std::to_string( _model->getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( ( "Residue: " + _model->getSymbolName() ).c_str(), flags ) )
				{
					/*
					bool isVisible = _model->isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::VisibleChangeVisibility(
							*_model, (Action::VisibleChangeVisibility::VISIBILITY_MODE)isVisible ) );
					}
					*/
					ImGui::Text( "ID: %d", _model->getIndex() );
					ImGui::Text( "Atoms: %d", _model->getAtomCount() );
					Color::Rgb color = _model->getColor();
					if ( ImGui::ColorEdit3( "Color", (float *)&color ) )
					{
						VTX_ACTION( new Action::Residue::ChangeColor( *_model, color ) );
						// VTX_ACTION( new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::RESIDUE ) );
					}
					Util::UI::drawRepresentations( *_model, *_model->getMoleculePtr() );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
