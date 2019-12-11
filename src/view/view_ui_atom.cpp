#include "view_ui_atom.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIAtom::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIAtom::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewAtom" );
			if ( ImGui::CollapsingHeader( ( "Atom: " + _model->getSymbolName() ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _model->getId() );
				ImGui::Text( "Vdw radius: %.2f", _model->getVdwRadius() );
				if ( ImGui::ColorEdit3( "Color", _model->_fColor ) ) {}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
