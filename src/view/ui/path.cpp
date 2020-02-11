#include "path.hpp"
#include "action/viewpoint_create.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Path::_draw()
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushID( "ViewPath" );
				if ( ImGui::CollapsingHeader( ( "Path: " + std::to_string( _getModel().getId() ) ).c_str(), flags ) )
				{
					ImGui::Text( "Duration: %f", _getModel().computeTotalTime() );
					ImGui::Text( "Checkpoints: %d", _getModel().getViewpoints().size() );
					if ( ImGui::Button( "Play" ) )
					{ VTXApp::get().goToState( ID::State::PLAYING, VTXApp::get().getScene().getPaths()[ 0 ] ); }
					ImGui::SameLine();
					if ( ImGui::Button( "Stop" ) ) { VTXApp::get().goToState( ID::State::VISUALIZATION ); }
					if ( ImGui::Button( "Add checkpoint" ) )
					{
						VTXApp::get().action(
							new Action::ViewpointCreate( _getModel(), VTXApp::get().getScene().getCamera() ) );
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
