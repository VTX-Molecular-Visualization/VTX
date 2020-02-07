#include "view_ui_path.hpp"
#include "action/action_checkpoint_create.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUIPath::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewPath" );
			if ( ImGui::CollapsingHeader( ( "Path: " + std::to_string( _getModel().getId() ) ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _getModel().getId() );
				ImGui::Text( "Checkpoints: %d", _getModel().getCheckpoints().size() );
				if ( ImGui::Button( "Play" ) )
				{ VTXApp::get().goToState( ID::State::PLAYING, VTXApp::get().getScene().getPaths()[ 0 ] ); }
				if ( ImGui::Button( "Add CP" ) )
				{
					VTXApp::get().action( new Action::ActionCheckpointCreate( VTXApp::get().getScene().getCamera() ) );
				}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
