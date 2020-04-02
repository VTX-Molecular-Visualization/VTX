#include "viewpoint.hpp"
#include "action/viewpoint_add_action.hpp"
#include "action/viewpoint_change_duration.hpp"
#include "action/viewpoint_delete.hpp"
#include "action/viewpoint_replace.hpp"
#include "object3d/scene.hpp"
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
				ImGui::PushID( ( "ViewViewpoint" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( LOCALE( "View.Viewpoint" ), flags ) )
				{
					if ( _getModel().getPathPtr()->getDurationMode() == Model::Path::DURATION_MODE::VIEWPOINT )
					{
						float duration = _getModel().getDuration();
						if ( ImGui::InputFloat( LOCALE( "View.Duration" ), &duration, 1.f ) )
						{ VTX_ACTION( new Action::ViewpointChangeDuration( _getModel(), duration ) ); }
					}
					else
					{
						ImGui::Text( "Duration: %f", _getModel().getDuration() );
					}

					if ( ImGui::Button( LOCALE( "View.Replace" ) ) )
					{
						VTX_ACTION( new Action::ViewpointReplace( _getModel(), VTXApp::get().getScene().getCamera() ) );
					}

					ImGui::SameLine();
					if ( ImGui::Button( LOCALE( "View.Delete" ) ) )
					{ VTX_ACTION( new Action::ViewpointDelete( _getModel() ) ); }

					ImGui::Text( LOCALE( "View.Actions" ) );
					for ( const std::string & action : _getModel().getActions() )
					{
						ImGui::Text( action.c_str() );
					}
					static char action[ 255 ] = "";
					ImGui::PushItemWidth( 255 );
					if ( ImGui::InputText(
							 "##Add", action, IM_ARRAYSIZE( action ), ImGuiInputTextFlags_EnterReturnsTrue ) )
					{ VTX_ACTION( new Action::ViewpointAddAction( _getModel(), std::string( action ) ) ); }

					ImGui::PopItemWidth();
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
