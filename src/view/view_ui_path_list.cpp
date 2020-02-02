#include "view_ui_path_list.hpp"
#include "action/action_checkpoint_create.hpp"
#include "action/action_checkpoint_goto.hpp"
#include "setting.hpp"
#include <string>

namespace VTX
{
	namespace View
	{
		void ViewUIPathList::_draw()
		{
			ImGui::PushID( "ViewPath" );
			if ( ImGui::CollapsingHeader( "Path", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				for ( Model::ModelCheckpoint * checkpoint : _getModel().getCheckpoints() )
				{
					ImGui::PushID( checkpoint->getId() );
					if ( ImGui::Selectable( "Checkpoint" ) )
					{
						VTXApp::get().action(
							new Action::ActionCheckpointGoTo( VTXApp::get().getScene().getCamera(), *checkpoint ) );
						//_getModel().setSelectedCheckpoint( checkpoint->getId() );
					}
					ImGui::SameLine();
					ImGui::InputFloat( "", checkpoint->getDurationPtr() );
					ImGui::PopID();
				}

				if ( ImGui::Button( "Add" ) )
				{
					VTXApp::get().action( new Action::ActionCheckpointCreate( VTXApp::get().getScene().getCamera() ) );
				}
				ImGui::SameLine();
				if ( ImGui::Button( "Play" ) )
				{ VTXApp::get().goToState( ID::State::PLAYING, VTXApp::get().getScene().getPaths()[ 0 ] ); }
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
