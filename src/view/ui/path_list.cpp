#include "path_list.hpp"
#include "action/selectable.hpp"
#include "action/viewpoint.hpp"
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
				bool pathOpened = ImGui::TreeNodeEx(
					"Path", _getModel().isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
				if ( ImGui::IsItemClicked() )
				{
					if ( pathOpened )
					{
						VTX_ACTION( new Action::Selectable::Unselect( _getModel() ) );
					}

					else
					{
						VTX_ACTION( new Action::Selectable::Select( _getModel() ) );
					}
				}
				if ( pathOpened )
				{
					uint i = 0;
					for ( Model::Viewpoint * const viewpoint : _getModel().getViewpoints() )
					{
						ImGui::PushID( viewpoint->getId() );
						if ( ImGui::Selectable( ( std::to_string( ++i ) + std::string( " - " )
												  + Util::Math::to_string( viewpoint->getPosition() )
												  + std::string( " - " ) + std::to_string( viewpoint->getDuration() )
												  + std::string( "s" ) )
													.c_str() ) )
						{
							VTX_ACTION(
								new Action::Viewpoint::GoTo( *viewpoint, VTXApp::get().getScene().getCamera() ) );

							if ( viewpoint->isSelected() )
							{
								VTX_ACTION( new Action::Selectable::Unselect( *viewpoint ) );
							}
							else
							{
								VTX_ACTION( new Action::Selectable::Select( *viewpoint ) );
							}
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
