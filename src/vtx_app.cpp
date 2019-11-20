#include "vtx_app.hpp"
#include "io/path.hpp"
#include "io/reader/reader_mmtf.hpp"
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
		_threads.push_back( std::thread( [&molecule ] {
			IO::ReaderMMTF mmtf = IO::ReaderMMTF();
			if ( mmtf.readFile( IO::Path( "../Vidocklab/VidockLab/data/"
										  "4v6x.mmtf" ),
								molecule ) )

			{
				molecule.init();
				molecule.printInfos();
			}
		} ) );
		//////////////

		while ( VTXApp::_isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		VTX_INFO( "Stopping application" );

		for ( std::thread & thread : _threads )
		{
			thread.join();
		}
		_threads.clear();

		VTX_INFO( "Application stopped" );
		VTXApp::_isRunning = false;
	}

	UI::BaseComponent * VTXApp::getUIComponentByType( UI::COMPONENT_TYPE p_type ) const
	{
		return _ui->getComponentByType( p_type );
	};

	void VTXApp::fireUIEvent( const Event::EVENT_UI p_event, void * const p_arg ) const
	{
		_ui->receiveEvent( p_event, p_arg );
	}

	void VTXApp::_update() { _ui->display(); }
} // namespace VTX
