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
				ImGui::PushID( ( "ViewPathList" + std::to_string( _model->getId() ) ).c_str() );
				bool pathOpened = ImGui::TreeNodeEx(
					"Path", _model->isSelected() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None );
				if ( ImGui::IsItemClicked() )
				{
					if ( pathOpened )
					{
						VTX_ACTION( new Action::Selectable::SetSelected( *_model, false ) );
					}

					else
					{
						VTX_ACTION( new Action::Selectable::SetSelected( *_model, true ) );
					}
				}
				if ( pathOpened )
				{
					uint i = 0;
					for ( Model::Viewpoint * const viewpoint : _model->getViewpoints() )
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
								VTX_ACTION( new Action::Selectable::SetSelected( *viewpoint, false ) );
							}
							else
							{
								VTX_ACTION( new Action::Selectable::SetSelected( *viewpoint, true ) );
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
