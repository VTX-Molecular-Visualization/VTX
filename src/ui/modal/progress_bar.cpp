#include "progress_bar.hpp"
#include "localization/language.hpp"
#include "style.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			void ProgressBar::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::UPDATE_PROGRESS_BAR )
				{
					_value = dynamic_cast<const Event::VTXEventFloat &>( p_event ).arg;

					if ( _value >= 1.f ) { _value = 0.f; }

					VTX_DEBUG( std::to_string( _value * 100 ) + "%" );
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
					vMax.x += IMGUI_STYLE_PROGRESS_BAR_SIZE.x * _value;
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
