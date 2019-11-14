#include "vtx_app.hpp"
#include "io/path.hpp"
#include "io/reader_mmtf.hpp"
#include "model/atom.hpp"
#include "model/model_molecule.hpp"
#include <thread>

namespace VTX
{
	bool VTXApp::_isRunning;

	VTXApp::VTXApp()
	{
		VTX_INFO( "Initializing application" );
		_ui = new UI::UserInterface();
		_ui->init();
		_scene	  = new Object3D::Scene();
		_renderer = new Renderer::Renderer();
	}

	VTXApp::~VTXApp()
	{
		delete _ui;
		delete _scene;
		delete _renderer;
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );
		VTXApp::_isRunning = true;
		VTX_INFO( "Application started" );
		_ui->printInfos();

		////////////// TESTS.
		Model::ModelMolecule molecule = Model::ModelMolecule();
		std::thread			 thread( [&molecule ] {
			 IO::ReaderMMTF mmtf = IO::ReaderMMTF();
			 mmtf.readFile(
				 IO::Path( "C:/Users/Samar/Desktop/VTX/Vidocklab/VidockLab/data/"
						   "3j3q.mmtf" ),
				 molecule );
			 molecule.printInfos();
		 } );
		//////////////

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
