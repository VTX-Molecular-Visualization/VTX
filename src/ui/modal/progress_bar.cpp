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

					if ( _value != 0.f )
					{
						VTX_DEBUG( std::to_string( _value * 100 ) + "%" );
					}
					if ( _value >= 1.f )
					{
						_value = 0.f;
					}
				}
			}

			bool ProgressBar::_drawHeader()
			{
				ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, IMGUI_STYLE_PROGRESS_BAR_SIZE );
				return BaseComponentModal::_drawHeader();
			}

			void ProgressBar::_drawFooter()
			{
				ImGui::PopStyleVar();

				// Open it.
				if ( _value != 0.f )
				{
					ImGui::OpenPopup( getTitle() );
					BaseComponentModal::_drawFooter();
				}
			}

			void ProgressBar::_drawContent()
			{
				ImVec2 vMin = ImGui::GetWindowContentRegionMin();
				ImVec2 vMax = ImGui::GetWindowContentRegionMax();

				vMin.x += ImGui::GetWindowPos().x;
				vMin.y += ImGui::GetWindowPos().y;
				vMax.x += IMGUI_STYLE_PROGRESS_BAR_SIZE.x * _value;
				vMax.y += ImGui::GetWindowPos().y;

				ImDrawList * drawList = ImGui::GetForegroundDrawList();
				drawList->AddRectFilled( vMin, vMax, IMGUI_STYLE_PROGRESS_BAR_COLOR );
			}
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
