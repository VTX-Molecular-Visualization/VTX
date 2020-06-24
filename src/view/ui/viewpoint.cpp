#include "viewpoint.hpp"
#include "action/viewpoint.hpp"
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
						{
							VTX_ACTION( new Action::Viewpoint::ChangeDuration( _getModel(), duration ) );
						}
					}
					else
					{
						ImGui::Text( "Duration: %f", _getModel().getDuration() );
					}

					if ( ImGui::Button( LOCALE( "View.Replace" ) ) )
					{
						VTX_ACTION(
							new Action::Viewpoint::Replace( _getModel(), VTXApp::get().getScene().getCamera() ) );
					}

					ImGui::SameLine();
					if ( ImGui::Button( LOCALE( "View.Delete" ) ) )
					{
						VTX_ACTION( new Action::Viewpoint::Delete( _getModel() ) );
					}

					ImGui::Text( LOCALE( "View.Actions" ) );

					uint i = 0;
					for ( std::vector<std::string>::const_iterator & action = _getModel().getActions().begin();
						  action != _getModel().getActions().end();
						  ++action )
					{
						ImGui::PushID( ( "ViewViewpointAction" + std::to_string( i ) ).c_str() );
						ImGui::Text( ( *action ).c_str() );
						if ( ImGui::Button( LOCALE( "View.Delete" ) ) )
						{
							VTX_ACTION( new Action::Viewpoint::DeleteAction( _getModel(), action ) );
						}
						i++;
						ImGui::PopID();
					}
					ImGui::PushItemWidth( 255 );
					if ( ImGui::InputText(
							 "##Add", _action, IM_ARRAYSIZE( _action ), ImGuiInputTextFlags_EnterReturnsTrue ) )
					{
						VTX_ACTION( new Action::Viewpoint::AddAction( _getModel(), std::string( _action ) ) );
					}

					ImGui::PopItemWidth();
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
