#include "app/worker/scene_loader.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/event.hpp"
#include "app/model/mesh_triangle.hpp"
#include "app/model/molecule.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/io/reader/lib_assimp.hpp"
#include "app/old_app/io/reader/lib_chemfiles.hpp"
#include "app/old_app/io/reader/prm.hpp"
#include "app/old_app/io/reader/psf.hpp"
#include "app/old_app/io/reader/serialized_object.hpp"
#include "app/old_app/io/struct/scene_path_data.hpp"
#include "app/old_app/object3d/camera.hpp"
#include "app/old_app/object3d/scene.hpp"
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

			IO::Reader::SerializedObject<VTXApp> * const reader = new IO::Reader::SerializedObject<VTXApp>();

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
