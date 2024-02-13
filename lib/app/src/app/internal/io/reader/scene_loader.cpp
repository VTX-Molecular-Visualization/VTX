#include "app/internal/io/reader/scene_loader.hpp"
#include "app/application/system/serializer.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::IO::Reader
{
	void SceneLoader::readFile( const FilePath & p_path, App::Application::Scene & p_scene )
	{
		SERIALIZER().readObject<App::Application::Scene>( p_path, p_scene );
	}

} // namespace VTX::App::Internal::IO::Reader
