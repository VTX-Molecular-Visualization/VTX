#ifndef __VTX_APP_UI_BASE_APPLICATION__
#define __VTX_APP_UI_BASE_APPLICATION__

#include "concepts.hpp"
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
		virtual ~BaseApplication() { _mainWindow.reset(); }

		void start( const App::Args & p_args ) override
		{
			// Create all the UI.
			_mainWindow = std::make_unique<MW>();
			_mainWindow->build();
			for ( Tool::BaseTool * const tool : _tools )
			{
				tool->createUI();
			}

			onUICreated();

			_mainWindow->prepare();

			// Start the main app.
			VTXApp::start( p_args );

			// Start the UI.
			_start();
		}

		inline static void addMenuAction( const WidgetId & p_menu, const App::UI::DescAction & p_action )
		{
			_mainWindow->addMenuAction( p_menu, p_action );
		}

		inline static void addToolBarAction( const WidgetId & p_toolbar, const App::UI::DescAction & p_action )
		{
			_mainWindow->addToolBarAction( p_toolbar, p_action );
		}

		inline static MW * const getMainWindow() { return _mainWindow.get(); }

		// Callbacks.
		inline static Util::Callback<> onUICreated;

	  protected:
		inline static std::unique_ptr<MW> _mainWindow;

		virtual void _start() = 0;
	};

} // namespace VTX::App::UI

namespace VTX
{
	// using APP_UI = App::UI::BaseApplication;
} // namespace VTX

#endif
