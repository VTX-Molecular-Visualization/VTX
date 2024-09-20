#include "app/serialization/io/reader/scene_loader.hpp"
#include "app/serialization/serialization_system.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Serialization::IO::Reader
{
	void SceneLoader::readFile( const FilePath & p_path, App::Application::Scene & p_scene )
	{
		SERIALIZATION_SYSTEM().readObject<App::Application::Scene>( p_path, p_scene );
	}

} // namespace VTX::App::Serialization::IO::Reader
