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
		BaseApplication( const App::Args & p_args ) : VTXApp( p_args ) {}

		virtual ~BaseApplication() { _mainWindow.reset(); }

		void start() override
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
			VTXApp::start();

			// Start the UI.
			_start();
		}

		inline MW * const getMainWindow() { return _mainWindow.get(); }

		// Callbacks.
		Util::Callback<> onUICreated;

	  protected:
		std::unique_ptr<MW> _mainWindow;

		virtual void _start() = 0;
	};

} // namespace VTX::App::UI

namespace VTX
{
	// using APP_UI = App::UI::BaseApplication;
} // namespace VTX

#endif
