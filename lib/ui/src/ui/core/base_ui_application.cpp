#include "ui/core/base_ui_application.hpp"
#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <python_binding/action.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Core
{
	BaseUIApplication::BaseUIApplication() : App::VTXApp() { VTX_DEBUG( "BaseUIApplication::BaseUIApplication()" ); }

	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		VTXApp::start( p_args );

		LayoutDescriptor layoutDescriptor;

		_init( p_args );
		_build( layoutDescriptor );
		_start( p_args );

		onStartUI();
	}

	void BaseUIApplication::stop() { VTXApp::stop(); }
} // namespace VTX::UI::Core
