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
		_initUI( p_args );
		_buildUI();
		_startUI( p_args );
	}

	void BaseUIApplication::_buildUI()
	{
		// TODO;
		/*
		const FilePath				  layoutPath = Util::Filesystem::getExecutableDir() / "data" / "tool_config.json";
		UI::Core::IO::VTXLayoutReader reader	 = UI::Core::IO::VTXLayoutReader();
		reader.read( layoutPath );

		UI::Core::LayoutBuilder layoutBuilder = UI::Core::LayoutBuilder();
		layoutBuilder.build( reader.getResult().layoutDescriptor );
		*/
	}

} // namespace VTX::UI::Core
