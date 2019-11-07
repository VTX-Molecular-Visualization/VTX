#include "vtx_app.hpp"
#include "io/path.hpp"
#include "io/reader_mmtf.hpp"
#include "model/model_molecule.hpp"

namespace VTX
{
	bool VTXApp::_isRunning;

	VTXApp::VTXApp()
	{
		VTX_INFO( "Initializing application" );
		_ui = new UI::UserInterface( NULL );
		_ui->init();
	}

	VTXApp::~VTXApp() { delete _ui; }

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );
		VTXApp::_isRunning = true;
		VTX_INFO( "Application started" );
		_ui->printInfos();

		// TESTS.
		Model::ModelMolecule molecule = Model::ModelMolecule();
		IO::ReaderMMTF *	 mmtf	  = new IO::ReaderMMTF();
		IO::Path path( "C:/Users/Simon/Documents/git/VTX/data/4v6x.mmtf" );
		mmtf->readFile( path, molecule );
		delete mmtf;
		//////

		while ( VTXApp::_isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		VTX_INFO( "Stopping application" );
		VTXApp::_isRunning = false;
	}

	template<typename T>
	void VTXApp::fireUIEvent( const Event::EVENT_UI p_name,
							  const T &				p_event ) const
	{
		_ui->receiveEvent<T>( p_name, p_event );
	}

	void VTXApp::_update() { _ui->display(); }
} // namespace VTX
