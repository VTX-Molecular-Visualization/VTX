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
					for ( Model::Viewpoint * const viewpoint : _getModel().getViewpoints() )
					{
						ImGui::PushID( viewpoint->getId() );
						if ( ImGui::Selectable( ( "Viewpoint " + std::to_string( viewpoint->getId() ) ).c_str() )/*,
						 viewpoint->isSelected()*/ )
						{
							VTXApp::get().action(
								new Action::ViewpointGoTo( *viewpoint, VTXApp::get().getScene().getCamera() ) );
							_getModel().setSelectedViewpoint( viewpoint );
						}
						ImGui::PopID();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
