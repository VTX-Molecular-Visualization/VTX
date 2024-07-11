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
		virtual ~BaseApplication() { VTX_DEBUG( "~BaseApplication()" ); }

		void start( const App::Args & p_args ) override
		{
			_init( p_args );
			_mainWindow = std::make_unique<MW>();
			_mainWindow->build();

			VTXApp::start( p_args );
			_start();
		}

		inline void addMenuAction( const WidgetId & p_menu, const WidgetId & p_action )
		{
			_mainWindow->addMenuAction( p_menu, p_action );
		}

		inline void addToolBarAction( const WidgetId & p_toolbar, const WidgetId & p_action )
		{
			_mainWindow->addToolBarAction( p_toolbar, p_action );
		}

		inline MW * const getMainWindow() { return _mainWindow.get(); }

	  protected:
		std::unique_ptr<MW> _mainWindow;

		virtual void _init( const App::Args & ) = 0;
		virtual void _start()					= 0;
	};

} // namespace VTX::UI

#endif
