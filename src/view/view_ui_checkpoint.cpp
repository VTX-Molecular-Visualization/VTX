#include "view_ui_checkpoint.hpp"
#include "../action/action_checkpoint_change_duration.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		void ViewUICheckpoint::_draw()
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID( "ViewCheckpoint" );
			if ( ImGui::CollapsingHeader( ( "Checkpoint: " + std::to_string( _getModel().getId() ) ).c_str(), flags ) )
			{
				ImGui::Text( "ID: %d", _getModel().getId() );
				float duration = _getModel().getDuration();
				if ( ImGui::InputFloat( "Duration", &duration, 1.f ) )
				{ VTXApp::get().action( new Action::ActionCheckpointChangeDuration( _getModel(), duration ) ); }
				if ( ImGui::Button( "Replace" ) ) {}
				if ( ImGui::Button( "Delete" ) ) {}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
