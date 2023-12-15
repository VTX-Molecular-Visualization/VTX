#include "app/action/application.hpp"
#include "app/application/scene.hpp"
#include "app/internal/io/reader/scene_loader.hpp"
#include "app/internal/io/writer/scene_writer.hpp"
#include "app/vtx_app.hpp"

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
} // namespace VTX::App::Action::Application
