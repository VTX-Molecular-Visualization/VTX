#include "ui/core/base_ui_application.hpp"
#include "ui/core/io/vtx_layout_reader.hpp"
#include "ui/core/layout_builder.hpp"
#include <app/old/action/main.hpp>
#include <app/old/internal/io/filesystem.hpp>
#include <app/old/vtx_app.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Core
{
	BaseUIApplication::BaseUIApplication() {}

	void BaseUIApplication::init() {}
	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: {}", VTX::App::Old::Internal::IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

		_initVTXApp( p_args );
		_initUI( p_args );
		_buildUI();
		_handleArgs( p_args );

		_postInit( p_args );
	}

	void BaseUIApplication::_initVTXApp( const std::vector<std::string> & p_args )
	{
		App::Old::VTXApp::get().start( p_args );
	}
	void BaseUIApplication::_postInit( const std::vector<std::string> & p_args )
	{
#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION(
			//	 new App::Old::Action::Main::Open( Util::Filesystem::getDataPath( FilePath( "4hhb.pdb" ) ).absolute() ) );
			VTX_ACTION( new App::Old::Action::Main::OpenApi( "1aga" ) );
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

	void BaseUIApplication::update() { App::Old::VTXApp::get().update(); }

	void BaseUIApplication::stop() { App::Old::VTXApp::get().stop(); }

	void BaseUIApplication::quit() {};

	void BaseUIApplication::_handleArgs( const std::vector<std::string> & p_args ) {}

} // namespace VTX::UI::Core
