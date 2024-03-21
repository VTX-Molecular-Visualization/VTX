#include "ui/core/base_ui_application.hpp"
#include "ui/core/io/vtx_layout_reader.hpp"
#include "ui/core/layout_builder.hpp"
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
	BaseUIApplication::BaseUIApplication() {}

	void BaseUIApplication::init() {}
	void BaseUIApplication::start( const std::vector<std::string> & p_args )
	{
		_initVTXApp( p_args );
		_initUI( p_args );
		_buildUI();
		_startUI( p_args );

		_postInit( p_args );
	}

	void BaseUIApplication::_initVTXApp( const std::vector<std::string> & p_args )
	{
		App::VTXApp::get().start( p_args );
	}

	void BaseUIApplication::_buildUI()
	{
		const FilePath				  layoutPath = Util::Filesystem::getExecutableDir() / "data" / "tool_config.json";
		UI::Core::IO::VTXLayoutReader reader	 = UI::Core::IO::VTXLayoutReader();
		reader.read( layoutPath );

		UI::Core::LayoutBuilder layoutBuilder = UI::Core::LayoutBuilder();
		layoutBuilder.build( reader.getResult().layoutDescriptor );
	}

	void BaseUIApplication::_postInit( const std::vector<std::string> & p_args )
	{
		using FILE_TYPE_ENUM = VTX::IO::Internal::Filesystem::FILE_TYPE_ENUM;
		for ( const std::string & p_arg : p_args )
		{
			// If argument is an existing file
			if ( std::filesystem::exists( p_arg ) )
			{
				const FilePath		 path	  = FilePath( p_arg );
				const FILE_TYPE_ENUM fileType = VTX::IO::Internal::Filesystem::getFileTypeFromFilePath( path );

				try
				{
					switch ( fileType )
					{
					case FILE_TYPE_ENUM::MOLECULE:
					case FILE_TYPE_ENUM::TRAJECTORY:
						App::VTX_ACTION().execute<App::Action::Scene::LoadMolecule>( p_arg );
						break;

					case FILE_TYPE_ENUM::SCENE:
						App::VTX_ACTION().execute<App::Action::Application::OpenScene>( p_arg );
						break;

					case FILE_TYPE_ENUM::SCRIPT:
						App::VTX_ACTION().execute<PythonBinding::Action::RunScript>( p_arg );
						break;

					default: throw IOException( "Unrecognized file" );
					}
				}
				catch ( const IOException & p_e )
				{
					VTX_ERROR( "Can't open file '{}' : {}.", p_arg, p_e.what() );
					break;
				}
			}
		}

#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION(
			//	 new App::Old::Action::Main::Open( Util::Filesystem::getDataPath( FilePath( "4hhb.pdb" ) ).absolute() )
			//);
			// App::Application::VTX_ACTION( new App::Old::Action::Main::OpenApi( "1aga" ) );
		}
#endif
		onInitEnded();
	}

	void BaseUIApplication::update() { App::VTXApp::get().update(); }

	void BaseUIApplication::stop() { App::VTXApp::get().stop(); }

	void BaseUIApplication::quit() {};

} // namespace VTX::UI::Core
