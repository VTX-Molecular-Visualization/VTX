#ifndef __VTX_APP_UI_BASE_APPLICATION__
#define __VTX_APP_UI_BASE_APPLICATION__

#include "concepts.hpp"
#include <any>
#include <app/vtx_app.hpp>
#include <string>
#include <util/logger.hpp>
#include <vector>

namespace VTX::App::UI
{
	template<ConceptMainWindow MW>
	class BaseApplication : public App::VTXApp
	{
	  public:
		BaseApplication() : VTXApp() {}
		virtual ~BaseApplication() { delete _mainWindow; }

		void start( const App::Args & p_args ) override
		{
			// Create all the UI.
			_mainWindow = new MW();
			_mainWindow->build();
			for ( Tool::BaseTool * const tool : _tools )
			{
				tool->createUI();
			}

			// Start the main app.
			VTXApp::start( p_args );

			// Start the UI.
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

		// inline static Util::Callback<> onUI;

	  protected:
		inline static MW * _mainWindow;

		virtual void _start() = 0;
	};

} // namespace VTX::App::UI

#endif
