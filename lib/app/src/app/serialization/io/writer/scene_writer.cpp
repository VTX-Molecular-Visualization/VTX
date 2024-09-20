#include "app/serialization/io/writer/scene_writer.hpp"
#include "app/application/scene.hpp"
#include "app/serialization/serialization_system.hpp"

namespace VTX::App::Serialization::IO::Writer
{
	void SceneWriter::writeFile( const FilePath & p_path, const App::Application::Scene & p_scene )
	{
		SERIALIZATION_SYSTEM().writeObject<Application::Scene>( p_path, p_scene );
	}

} // namespace VTX::App::Serialization::IO::Writer
