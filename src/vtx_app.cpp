#include "vtx_app.hpp"

namespace VTX
{
	VTXApp::VTXApp() { INFO( "Initializing application" ); }

	VTXApp::~VTXApp() {}

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

	void VTXApp::_update() { _ui.display(); }
} // namespace VTX
