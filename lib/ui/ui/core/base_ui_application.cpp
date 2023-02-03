#include "base_ui_application.hpp"
#include <src/action/action_manager.hpp>
#include "io/vtx_layout_reader.hpp"
#include "layout_builder.hpp"
#include <src/action/main.hpp>
#include <src/tool/logger.hpp>
#include <util/filepath.hpp>
#include <src/util/filesystem.hpp>
#include <src/vtx_app.hpp>

namespace VTX::UI::Core
{
	BaseUIApplication::BaseUIApplication() {}

	void BaseUIApplication::init() {}
	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::getExecutableFile().path() );

		_initVTXApp( p_args );
		_initUI( p_args );
		_buildUI();
		_handleArgs( p_args );

		_postInit( p_args );
	}

	void BaseUIApplication::_initVTXApp( const std::vector<std::string> & p_args ) { VTXApp::get().start( p_args ); }
	void BaseUIApplication::_postInit( const std::vector<std::string> & p_args )
	{
#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION(
			//	 new Action::Main::Open( Util::Filesystem::getDataPath( Util::FilePath( "4hhb.pdb" ) ).absolute() ) );
			VTX_ACTION( new Action::Main::OpenApi( "1aga" ) );
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

	void BaseUIApplication::update() { VTXApp::get().update(); }

	void BaseUIApplication::stop() { VTXApp::get().stop(); }

	void BaseUIApplication::quit() {};

	void BaseUIApplication::_handleArgs( const std::vector<std::string> & p_args )
	{
		std::vector<Util::FilePath> files  = std::vector<Util::FilePath>();
		std::vector<std::string>	pdbIds = std::vector<std::string>();

		for ( const std::string & arg : p_args )
		{
			if ( arg.find( "." ) != std::string::npos )
			{
				files.emplace_back( Util::FilePath( arg ) );
			}
			else
			{
				pdbIds.emplace_back( arg );
			}
		}

		VTX_ACTION( new Action::Main::Open( files ) );

		for ( const std::string & pdbId : pdbIds )
		{
			VTX_ACTION( new Action::Main::OpenApi( pdbId ) );
		}
	}

} // namespace VTX::UI::Core
