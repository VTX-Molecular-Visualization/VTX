#include "ui/core/base_ui_application.hpp"
#include "ui/core/io/vtx_layout_reader.hpp"
#include "ui/core/layout_builder.hpp"
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Core
{
	BaseUIApplication::BaseUIApplication() {}

	void BaseUIApplication::init() {}
	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		_initVTXApp( p_args );
		_initUI( p_args );
		_buildUI();
		_handleArgs( p_args );

		_postInit( p_args );
	}

	void BaseUIApplication::_initVTXApp( const std::vector<std::string> & p_args )
	{
		App::VTXApp::get().start( p_args );
	}
	void BaseUIApplication::_postInit( const std::vector<std::string> & p_args )
	{
#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION(
			//	 new App::Old::Action::Main::Open( Util::Filesystem::getDataPath( FilePath( "4hhb.pdb" ) ).absolute() )
			//);
			// App::Application::VTX_ACTION( new App::Old::Action::Main::OpenApi( "1aga" ) );
		}
#endif
	}

	void BaseUIApplication::_buildUI()
	{
		UI::Core::IO::VTXLayoutReader reader = UI::Core::IO::VTXLayoutReader();
		reader.read();

		UI::Core::LayoutBuilder layoutBuilder = UI::Core::LayoutBuilder();
		layoutBuilder.build( reader.getResult().layoutDescriptor );
	}

	void BaseUIApplication::update() { App::VTXApp::get().update(); }

	void BaseUIApplication::stop() { App::VTXApp::get().stop(); }

	void BaseUIApplication::quit() {};

	void BaseUIApplication::_handleArgs( const std::vector<std::string> & p_args ) {}

} // namespace VTX::UI::Core