#ifndef __VTX_UI_BASE_APPLICATION__
#define __VTX_UI_BASE_APPLICATION__

#include "actions.hpp"
#include "concepts.hpp"
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
		BaseApplication() : VTXApp() {}
		virtual ~BaseApplication() {}

		void start( const App::Args & p_args ) override
		{
			//_init( p_args );
			_mainWindow = std::make_unique<MW>();
			_mainWindow->build();
			onUI();

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

		inline static Util::Callback<> onUI;

	  protected:
		std::unique_ptr<MW> _mainWindow;

		// virtual void _init( const App::Args & ) = 0;
		virtual void _start() = 0;
	};

} // namespace VTX::UI

#endif
