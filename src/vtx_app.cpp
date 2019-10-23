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
		_isRunning = true;

		while ( _isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		INFO( "Stopping application" );
		_isRunning = false;
	}

	void VTXApp::_update() { _ui->display(); }
} // namespace VTX
