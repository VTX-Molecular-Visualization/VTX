#include "app/internal/io/writer/scene_writer.hpp"
#include "app/application/scene.hpp"
#include "app/application/system/serializer.hpp"

namespace VTX::App::Internal::IO::Writer
{
	void SceneWriter::writeFile( const FilePath & p_path, const App::Application::Scene & p_scene )
	{
		SERIALIZER().writeObject<Application::Scene>( p_path, p_scene );
	}

} // namespace VTX::App::Internal::IO::Writer
