#ifndef __VTX_UI_BASE_APPLICATION__
#define __VTX_UI_BASE_APPLICATION__

#include "concepts.hpp"
#include "descriptors.hpp"
#include <any>
#include <app/vtx_app.hpp>
#include <string>
#include <util/logger.hpp>
#include <vector>

namespace VTX::UI
{
	template<ConceptMainWindow MW>
	class BaseApplication : public App::VTXApp
	{
	  public:
		BaseApplication() : VTXApp() { VTX_DEBUG( "BaseApplication()" ); }
		virtual ~BaseApplication() {}

		void start( const App::Args & p_args ) override
		{
			VTXApp::start( p_args );

			_init( p_args );
			_mainWindow = std::make_unique<MW>();
			_mainWindow->build();
			_start();
		}
		void stop() override
		{
			_stop();

			VTXApp::stop();
		}

		void addMenuAction( const MenuAction & p_ma ) { _mainWindow->addMenuAction( p_ma ); }
		void addToolBarAction( const ToolBarAction & p_tba ) { _mainWindow->addToolBarAction( p_tba ); }

	  protected:
		std::unique_ptr<MW> _mainWindow;

		virtual void _init( const App::Args & ) = 0;
		virtual void _start()					= 0;
		virtual void _stop()					= 0;
	};

} // namespace VTX::UI

#endif
