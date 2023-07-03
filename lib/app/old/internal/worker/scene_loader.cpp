#include "app/old/internal/worker/scene_loader.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/object3d/mesh_triangle.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/core/io/reader/serialized_object.hpp"
#include "app/old/event.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/internal/io/reader/lib_assimp.hpp"
#include "app/old/internal/io/reader/lib_chemfiles.hpp"
#include "app/old/internal/io/reader/prm.hpp"
#include "app/old/internal/io/reader/psf.hpp"
#include "app/old/internal/io/serialization/scene_path_data.hpp"
#include "app/old/mvc.hpp"
#include "app/old/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Worker
{
	void SceneLoader::_run()
	{
		Util::Chrono chrono;

		for ( const FilePath & path : _paths )
		{
			chrono.start();
			VTX_INFO( "Loading {}", path.filename().string() );

			App::Old::Core::IO::Reader::SerializedObject<VTXApp> * const reader
				= new App::Old::Core::IO::Reader::SerializedObject<VTXApp>();

			try
			{
				App::Old::VTXApp::get().getScenePathData().setCurrentPath( path, false );
				reader->readFile( path, App::Old::VTXApp::get() );
				VTX_INFO( "App::Old loaded " );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load app: {}", p_e.what() );
			}

			delete reader;

			chrono.stop();
			VTX_INFO( "File treated in {}", chrono.elapsedTimeStr() );
		}
	}
} // namespace VTX::App::Old::Internal::Worker
