#include "view_ui_atom.hpp"
#include "../vtx_app.hpp"

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
				if ( ImGui::ColorEdit3( "Color", _model->_fColor ) )
				{
					_model->setColor( Vec3f( _model->_fColor[ 0 ], _model->_fColor[ 1 ], _model->_fColor[ 2 ] ) );
					Setting::Rendering::colorMode = View::MOLECULE_COLOR_MODE::ATOM;
					_model->getMoleculePtr()->setColorMode();
				}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
