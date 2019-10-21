#include "vtx_app.hpp"
#include <imgui.h>

namespace VTX
{
	VTXApp::VTXApp() { this->init(); }

	void VTXApp::init() { INFO( "Initializing app" ); }

	void VTXApp::start()
	{
		INFO( "Starting app" );
		/*
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui::Begin( "Hello, world!" );
		*/
	}

	void VTXApp::stop()
	{
		INFO( "Stopping app" );
		/*ImGui::DestroyContext();*/
	}
} // namespace VTX
