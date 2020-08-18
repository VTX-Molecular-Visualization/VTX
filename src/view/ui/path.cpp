#include "path.hpp"
#include "action/path.hpp"
#include "action/viewpoint.hpp"
#include "state/state_machine.hpp"
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
				ImGui::PushID( ( "ViewPath" + std::to_string( _model->getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( LOCALE( "View.Path" ), flags ) )
				{
					ImGui::Text( "Viewpoints: %d", _model->getViewpoints().size() );

					const char * durationModes[] = {
						LOCALE( "Enum.DurationMode.ConstantSpeed" ),
						LOCALE( "Enum.DurationMode.Path" ),
						LOCALE( "Enum.DurationMode.Viewpoint" ),
					};
					int durationMode = (int)_model->getDurationMode();
					if ( ImGui::Combo( LOCALE( "View.DurationMode" ), &durationMode, durationModes, 3 ) )
					{
						VTX_ACTION(
							new Action::Path::ChangeDurationMode( *_model, (Model::Path::DURATION_MODE)durationMode ) );
					}

					const char * interpolationModes[] = {
						LOCALE( "Enum.InterpolationMode.Linear" ),
						LOCALE( "Enum.InterpolationMode.CatmullRom" ),
						LOCALE( "Enum.InterpolationMode.Cubic" ),
					};
					int interpolationMode = (int)_model->getInterpolationMode();
					if ( ImGui::Combo( LOCALE( "View.InterpolationMode" ), &interpolationMode, interpolationModes, 2 ) )
					{
						VTX_ACTION( new Action::Path::ChangeInterpolationMode(
							*_model, (Model::Path::INTERPOLATION_MODE)interpolationMode ) );
					}

					if ( _model->getDurationMode() != Model::Path::DURATION_MODE::VIEWPOINT )
					{
						float duration = _model->getDuration();
						if ( ImGui::InputFloat( LOCALE( "View.Duration" ), &duration, 1.f ) )
						{
							VTX_ACTION( new Action::Path::ChangeDuration( *_model, duration ) );
						}
					}
					else
					{
						ImGui::Text( "Duration: %f", _model->getDuration() );
					}

					if ( ImGui::Button( LOCALE( "View.Play" ) ) )
					{
						VTX_ACTION( new Action::Path::Play( &*_model ) );
					}
					ImGui::SameLine();
					if ( ImGui::Button( LOCALE( "View.Stop" ) ) )
					{
						VTXApp::get().goToState( ID::State::VISUALIZATION );
					}
					ImGui::SameLine();
					bool isLooping = _model->isLooping();
					if ( ImGui::Checkbox( LOCALE( "View.Loop" ), &isLooping ) )
					{
						VTX_ACTION( new Action::Path::ChangeIsLooping( *_model, isLooping ) );
					}
					if ( ImGui::Button( LOCALE( "View.Viewpoint.Add" ) ) )
					{
						VTX_ACTION( new Action::Viewpoint::Create(
							*_model,
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
