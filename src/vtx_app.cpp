#include "vtx_app.hpp"
#include <imgui.h>

namespace VTX
{
	void VTXApp::start()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui::Begin( "Hello, world!" );
	}

	void VTXApp::stop()
	{
		ImGui::DestroyContext();
	}
} // namespace VTX
