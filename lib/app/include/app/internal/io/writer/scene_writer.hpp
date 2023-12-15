#ifndef __VTX_APP_INTERNAL_IO_WRITER_SCENE_WRITER__
#define __VTX_APP_INTERNAL_IO_WRITER_SCENE_WRITER__

#include "app/application/_fwd.hpp"
#include <util/types.hpp>

namespace VTX::App::Internal::IO::Writer
{
	class SceneWriter // : public VTX::IO::Core::Writer::BaseWriter<App::Application::Scene>
	{
	  public:
		SceneWriter() {};

		void writeFile( const FilePath & p_path, const App::Application::Scene & p_scene );
	};
} // namespace VTX::App::Internal::IO::Writer
#endif
