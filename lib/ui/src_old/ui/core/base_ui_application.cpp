#include "ui/core/base_ui_application.hpp"
#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Core
{
	BaseUIApplication::BaseUIApplication() : App::VTXApp() {}

	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		VTXApp::start( p_args );

		LayoutDescriptor layoutDescriptor
			= { LayoutDescriptor::Buttons { { "button1", "tab1", "block1" }, { "button2", "tab2", "block2" } } };

		_init( p_args );
		_build( layoutDescriptor );
		_start( p_args );

		onStartUI();
	}

	void BaseUIApplication::stop()
	{
		// Stop UI before App?
		_stop();
		VTXApp::stop();
	}
} // namespace VTX::UI::Core
