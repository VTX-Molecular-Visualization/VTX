#include "path.hpp"
#include "action/path_change_duration.hpp"
#include "action/path_change_duration_mode.hpp"
#include "action/path_play.hpp"
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
				if ( ImGui::CollapsingHeader( ( LOCALE( "View.Path" ) + std::to_string( _getModel().getId() ) ).c_str(),
											  flags ) )
				{
					ImGui::Text( "Viewpoints: %d", _getModel().getViewpoints().size() );

					const char * modes[] = {
						LOCALE( "Enum.DurationMode.ConstantSpeed" ),
						LOCALE( "Enum.DurationMode.Path" ),
						LOCALE( "Enum.DurationMode.Viewpoint" ),
					};
					int mode = (int)_getModel().getDurationMode();
					if ( ImGui::Combo( LOCALE( "View.DurationMode" ), &mode, modes, 3 ) )
					{
						VTXApp::get().action(
							new Action::PathChangeDurationMode( _getModel(), (Model::Path::DURATION_MODE)mode ) );
					}

					if ( _getModel().getDurationMode() != Model::Path::DURATION_MODE::VIEWPOINT )
					{
						float duration = _getModel().getDuration();
						if ( ImGui::InputFloat( LOCALE( "View.Duration" ), &duration, 1.f ) )
						{ VTXApp::get().action( new Action::PathChangeDuration( _getModel(), duration ) ); }
					}
					else
					{
						ImGui::Text( "Duration: %f", _getModel().getDuration() );
					}

					if ( ImGui::Button( LOCALE( "View.Play" ) ) )
					{ VTXApp::get().action( new Action::PathPlay( &_getModel() ) ); }
					ImGui::SameLine();
					if ( ImGui::Button( LOCALE( "View.Stop" ) ) )
					{ VTXApp::get().goToState( ID::State::VISUALIZATION ); }
					if ( ImGui::Button( LOCALE( "View.Viewpoint.Add" ) ) )
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
