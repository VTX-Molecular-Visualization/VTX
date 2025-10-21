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
		BaseApplication( const App::Args & p_args ) : VTXApp( p_args )
		{
			// Create all the UI.
			_mainWindow = std::make_unique<MW>();
			_mainWindow->build();
			_mainWindow->prepare();
		}

		virtual ~BaseApplication() { _mainWindow.reset(); }

		void start() override
		{
			// Start the main app.
			VTXApp::start();

			// Start the UI.
			_start();
		}

		inline MW * const getMainWindow() { return _mainWindow.get(); }

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
