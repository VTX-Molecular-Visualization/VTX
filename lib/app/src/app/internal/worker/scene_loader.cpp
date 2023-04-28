#include "app/internal/worker/scene_loader.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/object3d/mesh_triangle.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/event.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/internal/io/reader/lib_assimp.hpp"
#include "app/internal/io/reader/lib_chemfiles.hpp"
#include "app/internal/io/reader/prm.hpp"
#include "app/internal/io/reader/psf.hpp"
#include "app/internal/io/serialization/scene_path_data.hpp"
#include "app/mvc.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::Worker
{
	void SceneLoader::_run()
	{
		Util::Chrono chrono;

		for ( const FilePath & path : _paths )
		{
			chrono.start();
			VTX_INFO( "Loading " + path.filename().string() );

			App::Core::IO::Reader::SerializedObject<VTXApp> * const reader
				= new App::Core::IO::Reader::SerializedObject<VTXApp>();

			try
			{
				VTXApp::get().getScenePathData().setCurrentPath( path, false );
				reader->readFile( path, VTXApp::get() );
				VTX_INFO( "App loaded " );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load app: " + std::string( p_e.what() ) );
			}

			delete reader;

			chrono.stop();
			VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}
	}
} // namespace VTX::Worker
