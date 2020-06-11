#include "progress_bar.hpp"
#include "localization/language.hpp"
#include "util/logger.hpp"

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

					if ( _value >= 1.f )
					{
						_value = 0.f;
					}
				}
			}

			bool ProgressBar::_drawHeader()
			{
				// Open it.
				if ( _value != 0.f )
				{
					ImGui::OpenPopup( getTitle() );
				}

				ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, IMGUI_STYLE_PROGRESS_BAR_SIZE );
				return BaseComponentModal::_drawHeader();
			}

			void ProgressBar::_drawFooter() { ImGui::PopStyleVar(); }

			void ProgressBar::_drawContent()
			{
				ImVec2 position = ImGui::GetWindowPos();
				ImVec2 min		= ImGui::GetWindowContentRegionMin();
				ImVec2 max		= ImGui::GetWindowContentRegionMax();

				min.x += position.x;
				min.y += position.y;
				max.x = min.x + _value * IMGUI_STYLE_PROGRESS_BAR_SIZE.x;
				max.y += position.y;

				ImDrawList * drawList = ImGui::GetForegroundDrawList();
				drawList->AddRectFilled( min, max, IMGUI_STYLE_PROGRESS_BAR_COLOR );

				if ( _value == 0.f )
				{
					ImGui::CloseCurrentPopup();
				}

				// Called here instead of ProgressBar::_drawFooter() because global
				// footer need to be displayed only when content is displayed...
				BaseComponentModal::_drawFooter();
			}
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
