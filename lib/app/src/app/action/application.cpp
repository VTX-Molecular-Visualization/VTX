#include "app/action/application.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/filesystem.hpp"
#include "app/serialization/io/reader/scene_loader.hpp"
#include "app/serialization/io/writer/scene_writer.hpp"
#include "app/serialization/serialization_system.hpp"
#include <app/component/render/camera.hpp>

namespace VTX::App::Action::Application
{
	void NewScene::execute()
	{
		SCENE().reset();
		// App::Old::APP::getScenePathData().clearCurrentPath();

		APP::onStartBlockingOperation( "Opening files" );

		APP::onUpdateBlockingOperation( 0.f );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		APP::onUpdateBlockingOperation( 0.25f );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		APP::onUpdateBlockingOperation( 0.5f );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		APP::onUpdateBlockingOperation( 0.75f );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		APP::onUpdateBlockingOperation( 1.f );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		APP::onEndBlockingOperation();
	}

	void OpenScene::execute()
	{
		SCENE().clear();

		Serialization::IO::Reader::SceneLoader loader = Serialization::IO::Reader::SceneLoader();
		loader.readFile( _path, SCENE() );

		// App::Old::APP::getScenePathData().setCurrentPath( _path, true );
	}

	void SaveScene::execute()
	{
		Serialization::IO::Writer::SceneWriter writer = Serialization::IO::Writer::SceneWriter();
		writer.writeFile( _path, SCENE() );

		// VTX_THREAD( saver, _callback );

		// if ( _path.extension() == "vtx" )
		//{
		//	App::Old::APP::getScenePathData().setCurrentPath( _path, true );
		//	VTX_EVENT( VTX::App::Old::Event::Global::SCENE_SAVED );
		// }
		// else
		//{
		//	VTX::App::Old::Application::Setting::enqueueNewLoadingPath( _path );
		// }
	}
	void ClearScene::execute() { SCENE().reset(); }

	LoadSettings::LoadSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void LoadSettings::execute()
	{
		SERIALIZATION_SYSTEM().readObject<App::Core::Settings::SettingsSystem>( _path, SETTINGS_SYSTEM() );
	}
	SaveSettings::SaveSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void SaveSettings::execute()
	{
		SERIALIZATION_SYSTEM().writeObject<App::Core::Settings::SettingsSystem>( _path, SETTINGS_SYSTEM() );
	}
	void ReloadSettings::execute()
	{
		SERIALIZATION_SYSTEM().readObject<App::Core::Settings::SettingsSystem>(
			VTX::App::Filesystem::getSettingJsonFile(), SETTINGS_SYSTEM()
		);
	}
	void ResetSettings::execute() { SETTINGS_SYSTEM().reset(); }

	void Open::execute()
	{
		FilePath scenePath = "";
		for ( const FilePath & path : _paths )
		{
			if ( path.extension() == "vtx" )
			{
				scenePath = path;
				break;
			}
		}

		if ( !scenePath.empty() )
		{
			Action::Application::OpenScene openSceneAction = Action::Application::OpenScene( scenePath );
			openSceneAction.execute();
		}
		else
		{
			for ( const FilePath & moleculePath : _paths )
			{
				Action::Scene::LoadMolecule loadMoleculeAction = Action::Scene::LoadMolecule( moleculePath );
				loadMoleculeAction.execute();
			}
		}
	}

	void Quit::execute()
	{
		APP::onEndOfFrameOneShot += []() { APP::stop(); };
	}

	void Resize::execute()
	{
		App::SCENE().getCamera().setScreenSize( _width, _height );
		App::RENDERER_SYSTEM().resize( _width, _height, _output );
	}

	void RunScript::execute()
	{
		// INTERPRETOR().runScript( _path );

	} // namespace VTX::App::Action::Application
} // namespace VTX::App::Action::Application
