#include "base_component_modal.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			bool BaseComponentModal::_drawHeader()
			{
				ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_MODAL_BG_ALPHA );
				return ImGui::BeginPopupModal( getTitle(),
											   nullptr,
											   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												   | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove
												   | ImGuiWindowFlags_NoDecoration );
			}

			void BaseComponentModal::_drawFooter() { ImGui::EndPopup(); }
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
