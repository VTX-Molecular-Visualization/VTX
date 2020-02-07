#include "view_ui_path_list.hpp"
#include "action/action_checkpoint_goto.hpp"
#include "setting.hpp"
#include <string>

namespace VTX
{
	namespace View
	{
		void ViewUIPathList::_draw()
		{
			ImGui::PushID( "ViewPathList" );
			if ( ImGui::CollapsingHeader( "Path", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				for ( Model::ModelCheckpoint * const checkpoint : _getModel().getCheckpoints() )
				{
					ImGui::PushID( checkpoint->getId() );
					if ( ImGui::Selectable( ( "Checkpoint " + std::to_string( checkpoint->getId() ) ).c_str() )/*,
						 checkpoint->isSelected()*/ )
					{
						VTXApp::get().action(
							new Action::ActionCheckpointGoTo( *checkpoint, VTXApp::get().getScene().getCamera() ) );
						_getModel().setSelectedCheckpoint( checkpoint );
					}
					ImGui::PopID();
				}
			}
			ImGui::PopID();
		}
	} // namespace View
} // namespace VTX
