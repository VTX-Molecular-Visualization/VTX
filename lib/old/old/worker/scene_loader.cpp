#include "scene_loader.hpp"
#include "event/event_manager.hpp"
#include "io/reader/lib_assimp.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/reader/prm.hpp"
#include "io/reader/psf.hpp"
#include "io/reader/serialized_object.hpp"
#include "io/struct/scene_path_data.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"

namespace VTX::Worker
{
	void SceneLoader::_run()
	{
		Tool::Chrono chrono;

		for ( const Util::FilePath & path : _paths )
		{
			chrono.start();
			VTX_INFO( "Loading " + path.filename() );

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
