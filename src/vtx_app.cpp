#include "vtx_app.hpp"

namespace VTX
{
	VTXApp::VTXApp()
	{
		INFO( "Initializing app" );
		_ui = new UI::UserInterface();
	}

	VTXApp::~VTXApp() { delete _ui; }

	void VTXApp::start() { INFO( "Starting app" ); }

	void VTXApp::stop() { INFO( "Stopping app" ); }
} // namespace VTX
