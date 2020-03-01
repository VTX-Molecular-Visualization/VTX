#include "progress_bar.hpp"
#include "localization/language.hpp"
#include "style.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			void ProgressBar::receiveEvent( const Event::VTX_EVENT & p_event, void * const p_arg )
			{
				if ( p_event == Event::Global::UPDATE_PROGRESS_BAR )
				{
					_value = *(float *)p_arg;

					if ( _value >= 1.f ) { _value = 0.f; }
				}
			}

			void ProgressBar::_draw()
			{
				ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_PROGRESS_BG_ALPHA );
				ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, IMGUI_STYLE_PROGRESS_BAR_SIZE );
				if ( ImGui::BeginPopupModal( LOCALE( "Modal.Loading" ),
											 nullptr,
											 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												 | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove
												 | ImGuiWindowFlags_NoDecoration ) )
				{
					ImVec2 vMin = ImGui::GetWindowContentRegionMin();
					ImVec2 vMax = ImGui::GetWindowContentRegionMax();

					vMin.x += ImGui::GetWindowPos().x;
					vMin.y += ImGui::GetWindowPos().y;
					vMax.x += ImGui::GetWindowPos().x;
					vMax.y += ImGui::GetWindowPos().y;

					ImDrawList * drawList = ImGui::GetForegroundDrawList();
					drawList->AddRectFilled( vMin, vMax, IMGUI_STYLE_PROGRESS_BAR_COLOR );

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();

				// Open it.
				if ( _value != 0.f ) { ImGui::OpenPopup( LOCALE( "Modal.Loading" ) ); }
			}
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
