#include "app/internal/io/writer/scene_writer.hpp"
#include "app/application/scene.hpp"
#include "app/core/io/writer/serialized_object.hpp"

namespace VTX::App::Internal::IO::Writer
{
	void SceneWriter::writeFile( const FilePath & p_path, const App::Application::Scene & p_scene )
	{
		Core::IO::Writer::SerializedObject<App::Application::Scene> serializedScene
			= Core::IO::Writer::SerializedObject( p_path, &p_scene );

		serializedScene.write();
	}

} // namespace VTX::App::Internal::IO::Writer
