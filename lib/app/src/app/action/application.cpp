#include "app/action/application.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/application/system/serializer.hpp"
#include "app/application/system/settings_system.hpp"
#include "app/internal/io/reader/scene_loader.hpp"
#include "app/internal/io/writer/scene_writer.hpp"
#include "app/internal/serialization/all_serializers.hpp"
#include <io/internal/filesystem.hpp>

namespace VTX::App::Action::Application
{
	void NewScene::execute()
	{
		SCENE().reset();
		// App::Old::VTXApp::get().getScenePathData().clearCurrentPath();
	}

	void OpenScene::execute()
	{
		SCENE().clear();

		Internal::IO::Reader::SceneLoader loader = Internal::IO::Reader::SceneLoader();
		loader.readFile( _path, SCENE() );

		// App::Old::VTXApp::get().getScenePathData().setCurrentPath( _path, true );
	}

	void SaveScene::execute()
	{
		Internal::IO::Writer::SceneWriter writer = Internal::IO::Writer::SceneWriter();
		writer.writeFile( _path, SCENE() );

		// VTX_THREAD( saver, _callback );

		// if ( _path.extension() == "vtx" )
		//{
		//	App::Old::VTXApp::get().getScenePathData().setCurrentPath( _path, true );
		//	VTX_EVENT( VTX::App::Old::Event::Global::SCENE_SAVED );
		// }
		// else
		//{
		//	VTX::App::Old::Application::Setting::enqueueNewLoadingPath( _path );
		// }
	}
	void ClearScene::execute() { SCENE().reset(); }

	LoadSettings::LoadSettings() : _path( VTX::IO::Internal::Filesystem::getSettingJsonFile() ) {}
	void LoadSettings::execute() { SERIALIZER().readObject<App::Application::Settings::Settings>( _path, SETTINGS() ); }
	SaveSettings::SaveSettings() : _path( VTX::IO::Internal::Filesystem::getSettingJsonFile() ) {}
	void SaveSettings::execute()
	{
		SERIALIZER().writeObject<App::Application::Settings::Settings>( _path, SETTINGS() );
	}
	void ReloadSettings::execute()
	{
		SERIALIZER().readObject<App::Application::Settings::Settings>(
			VTX::IO::Internal::Filesystem::getSettingJsonFile(), SETTINGS()
		);
	}
	void ResetSettings::execute() { SETTINGS().reset(); }

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
} // namespace VTX::App::Action::Application
