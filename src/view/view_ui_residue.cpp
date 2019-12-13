#include "view_ui_residue.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIResidue::notify( Event::EVENT_MODEL p_event ) {}

		void ViewUIResidue::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewResidue" );
			if ( ImGui::CollapsingHeader( ( "Residue: " + _model->getSymbolName() ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _model->getId() );
				ImGui::Text( "Atoms: %d", _model->getAtomCount() );
				if ( ImGui::ColorEdit3( "Color", _model->_fColor ) )
				{
					_model->setColor( Vec3f( _model->_fColor[ 0 ], _model->_fColor[ 1 ], _model->_fColor[ 2 ] ) );
					Setting::Rendering::colorMode = View::MOLECULE_COLOR_MODE::RESIDUE;
					_model->getMoleculePtr()->setColorMode();
				}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
