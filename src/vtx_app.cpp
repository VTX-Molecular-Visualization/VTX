#include "vtx_app.hpp"

namespace VTX
{
	VTXApp::VTXApp()
	{
		INFO( "Initializing application" );
		_ui = new UI::UserInterface();
	}

	VTXApp::~VTXApp() { delete _ui; }

	void VTXApp::start()
	{
		INFO( "Starting application" );
		while ( 1 )
		{
			_ui->display();
		}
	}

	void VTXApp::stop() { INFO( "Stopping application" ); }
} // namespace VTX
