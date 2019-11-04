#include "vtx_app.hpp"

namespace VTX
{
	bool VTXApp::_isRunning;

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
		VTXApp::_isRunning = true;
		INF( "Application started" );
		_ui->printInfos();

		while ( VTXApp::_isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		INF( "Stopping application" );
		VTXApp::_isRunning = false;
	}

	void VTXApp::fireUIEvent( const Event::EVENT_UI p_event, ... ) const
	{
		va_list ap;
		va_start( ap, p_event );
		_ui->receiveEvent( p_event, ap );
		va_end( ap );
	}

	void VTXApp::_update() { _ui->display(); }
} // namespace VTX
