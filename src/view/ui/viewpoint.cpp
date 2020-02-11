#include "viewpoint.hpp"
#include "../../action/viewpoint_change_duration.hpp"
#include "../../action/viewpoint_delete.hpp"
#include "../../action/viewpoint_replace.hpp"
#include "../../object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Viewpoint::_draw()
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushID( "ViewViewpoint" );
				if ( ImGui::CollapsingHeader( ( "Viewpoint: " + std::to_string( _getModel().getId() ) ).c_str(),
											  flags ) )
				{
					float duration = _getModel().getDuration();
					if ( ImGui::InputFloat( "Duration", &duration, 1.f ) )
					{ VTXApp::get().action( new Action::ViewpointChangeDuration( _getModel(), duration ) ); }
					if ( ImGui::Button( "Replace" ) )
					{
						VTXApp::get().action(
							new Action::ViewpointReplace( _getModel(), VTXApp::get().getScene().getCamera() ) );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "Delete" ) )
					{ VTXApp::get().action( new Action::ViewpointDelete( _getModel() ) ); }
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
