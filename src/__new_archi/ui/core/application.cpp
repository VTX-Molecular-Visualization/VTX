#include "application.hpp"
#include "__new_archi/app/vtx_app_v2.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "io/vtx_layout_reader.hpp"
#include "layout_builder.hpp"
#include "tool/logger.hpp"
#include "util/filepath.hpp"
#include "util/filesystem.hpp"

namespace VTX::UI::Core
{
	void Application::init() {}
	void Application::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::getExecutableFile().path() );

		_initVTXApp( p_args );
		_initUI( p_args );
		_buildUI();
		_handleArgs( p_args );

		_postInit( p_args );
	}

	void Application::_initVTXApp( const std::vector<std::string> & p_args ) { App::VTXAppV2::get().start( p_args ); }
	void Application::_postInit( const std::vector<std::string> & p_args )
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

	void Application::_buildUI()
	{
		UI::Core::IO::VTXLayoutReader reader = UI::Core::IO::VTXLayoutReader();
		reader.read();

		UI::Core::LayoutBuilder layoutBuilder = UI::Core::LayoutBuilder();
		layoutBuilder.build( reader.getResult().layoutDescriptor );
	}

	void Application::update() { VTX::App::VTXAppV2::get().update(); }

	void Application::stop() { VTX::App::VTXAppV2::get().stop(); }

	void Application::quit() {};

	void Application::_handleArgs( const std::vector<std::string> & p_args )
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
