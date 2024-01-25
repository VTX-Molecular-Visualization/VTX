#include "app/internal/io/reader/scene_loader.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::IO::Reader
{
	void SceneLoader::readFile( const FilePath & p_path, App::Application::Scene & p_scene )
	{
		Core::IO::Reader::SerializedObject<App::Application::Scene> serializedScene
			= Core::IO::Reader::SerializedObject( p_path, &p_scene );

		serializedScene.read();
	}

} // namespace VTX::App::Internal::IO::Reader
