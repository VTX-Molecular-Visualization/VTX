#ifndef __VTX_APP_INTERNAL_IO_SCENE_LOADER__
#define __VTX_APP_INTERNAL_IO_SCENE_LOADER__

#include "app/application/scene.hpp"
#include <io/core/reader/base_reader.hpp>
#include <util/types.hpp>

namespace VTX::App::Internal::IO::Reader
{
	class SceneLoader : public VTX::IO::Core::Reader::BaseReader<App::Application::Scene>
	{
	  public:
		SceneLoader() : VTX::IO::Core::Reader::BaseReader<App::Application::Scene>() {};

		void readFile( const FilePath & p_path, App::Application::Scene & p_scene ) override;
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
