#include "path.hpp"
#include "action/path_change_duration.hpp"
#include "action/path_change_duration_mode.hpp"
#include "action/path_change_interpolation_mode.hpp"
#include "action/path_change_is_looping.hpp"
#include "action/path_play.hpp"
#include "action/viewpoint.hpp"
#include "state/visualization.hpp"
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
				ImGui::PushID( ( "ViewPath" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( LOCALE( "View.Path" ), flags ) )
				{
					ImGui::Text( "Viewpoints: %d", _getModel().getViewpoints().size() );

					const char * durationModes[] = {
						LOCALE( "Enum.DurationMode.ConstantSpeed" ),
						LOCALE( "Enum.DurationMode.Path" ),
						LOCALE( "Enum.DurationMode.Viewpoint" ),
					};
					int durationMode = (int)_getModel().getDurationMode();
					if ( ImGui::Combo( LOCALE( "View.DurationMode" ), &durationMode, durationModes, 3 ) )
					{
						VTX_ACTION( new Action::PathChangeDurationMode( _getModel(),
																		(Model::Path::DURATION_MODE)durationMode ) );
					}

					const char * interpolationModes[] = {
						LOCALE( "Enum.InterpolationMode.Linear" ),
						LOCALE( "Enum.InterpolationMode.CatmullRom" ),
						LOCALE( "Enum.InterpolationMode.Cubic" ),
					};
					int interpolationMode = (int)_getModel().getInterpolationMode();
					if ( ImGui::Combo( LOCALE( "View.InterpolationMode" ), &interpolationMode, interpolationModes, 2 ) )
					{
						VTX_ACTION( new Action::PathChangeInterpolationMode(
							_getModel(), (Model::Path::INNTERPOLATION_MODE)interpolationMode ) );
					}

					if ( _getModel().getDurationMode() != Model::Path::DURATION_MODE::VIEWPOINT )
					{
						float duration = _getModel().getDuration();
						if ( ImGui::InputFloat( LOCALE( "View.Duration" ), &duration, 1.f ) )
						{
							VTX_ACTION( new Action::PathChangeDuration( _getModel(), duration ) );
						}
					}
					else
					{
						ImGui::Text( "Duration: %f", _getModel().getDuration() );
					}

					if ( ImGui::Button( LOCALE( "View.Play" ) ) )
					{
						VTX_ACTION( new Action::PathPlay( &_getModel() ) );
					}
					ImGui::SameLine();
					if ( ImGui::Button( LOCALE( "View.Stop" ) ) )
					{
						VTXApp::get().goToState( ID::State::VISUALIZATION );
					}
					ImGui::SameLine();
					bool isLooping = _getModel().isLooping();
					if ( ImGui::Checkbox( LOCALE( "View.Loop" ), &isLooping ) )
					{
						VTX_ACTION( new Action::PathChangeIsLooping( _getModel(), isLooping ) );
					}
					if ( ImGui::Button( LOCALE( "View.Viewpoint.Add" ) ) )
					{
						VTX_ACTION( new Action::Viewpoint::Create(
							_getModel(),
							VTXApp::get().getScene().getCamera(),
							VTXApp::get()
								.getStateMachine()
								.getItem<State::Visualization>( ID::State::VISUALIZATION )
								->getCurrentController() ) );
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
