#include "vtx_app.hpp"

namespace VTX
{
	VTXApp::VTXApp()
	{
		INF( "Initializing application" );
		_ui = new UI::UserInterface( NULL );
		_ui->init();
	}

	VTXApp::~VTXApp() { delete _ui; }

	void VTXApp::start()
	{
		INF( "Starting application" );
		_isRunning = true;

		while ( _isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		INF( "Stopping application" );
		_isRunning = false;
	}

	void VTXApp::_update() { _ui->display(); }
} // namespace VTX
