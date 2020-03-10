#include "path_list.hpp"
#include "action/select.hpp"
#include "action/unselect.hpp"
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
				ImGui::PushID( ( "ViewPathList" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( "Path", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					uint i = 0;
					for ( Model::Viewpoint * const viewpoint : _getModel().getViewpoints() )
					{
						ImGui::PushID( viewpoint->getId() );
						if ( ImGui::Selectable( ( std::to_string( ++i ) 
							+ std::string(" - ") 
							+ glm::to_string(viewpoint->getPosition() )   
							+ std::string(" - ") 
							+ std::to_string(viewpoint->getDuration()) 
							+ std::string("s")).c_str()  ) )
						{
							VTXApp::get().getActionManager().execute(
								new Action::ViewpointGoTo( *viewpoint, VTXApp::get().getScene().getCamera() ) );

							if ( viewpoint->isSelected() )
							{ VTXApp::get().getActionManager().execute( new Action::Unselect( *viewpoint ) ); }
							else
							{
								VTXApp::get().getActionManager().execute( new Action::Select( *viewpoint ) );
							}
						}
						ImGui::PopID();
					}
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
