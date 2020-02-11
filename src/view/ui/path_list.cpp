#include "path_list.hpp"
#include "action/viewpoint_goto.hpp"
#include "setting.hpp"
#include <string>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void PathList::_draw()
			{
				ImGui::PushID( "ViewPathList" );
				if ( ImGui::CollapsingHeader( "Path", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					for ( Model::Viewpoint * const checkpoint : _getModel().getViewpoints() )
					{
						ImGui::PushID( checkpoint->getId() );
						if ( ImGui::Selectable( ( "Viewpoint " + std::to_string( checkpoint->getId() ) ).c_str() )/*,
						 checkpoint->isSelected()*/ )
						{
							VTXApp::get().action(
								new Action::ViewpointGoTo( *checkpoint, VTXApp::get().getScene().getCamera() ) );
							_getModel().setSelectedViewpoint( checkpoint );
						}
						ImGui::PopID();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
