#include "app/action/main.hpp"
#include "app/core/worker/worker_manager.hpp"
#include "app/internal/network/request/download_mmtf.hpp"
#include "app/manager/network_manager.hpp"
#include "app/network.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/object3d/camera_manager.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker/loader.hpp"
#include "app/worker/render_effect_loader.hpp"
#include "app/worker/representation_loader.hpp"
#include "app/worker/saver.hpp"
#include "app/worker/scene_loader.hpp"

namespace VTX::App::Action::Main
{
	void New::execute()
	{
		VTXApp::get().getScene().reset();
		VTXApp::get().getScenePathData().clearCurrentPath();
	}
	// TODO keep only Dialog parts here and move real loading action into VTX_APP.
	void Open::LoadSceneClass::_loadScene()
	{
		Worker::SceneLoader * sceneLoader = new Worker::SceneLoader( _paths );
		VTX_WORKER( sceneLoader );

		for ( const FilePath & path : _paths )
		{
			VTXApp::get().getScenePathData().setCurrentPath( path, true );
		}
	}
	void Open::execute()
	{
		bool loadScene = false;
		for ( const FilePath & path : _paths )
		{
			loadScene = loadScene || path.extension() == "vtx";
		}

		if ( loadScene )
		{
			if ( _paths.empty() )
				return;

			VTXApp::get().getScene().clear();

			LoadSceneClass * const sceneClass = new LoadSceneClass( _paths );
			sceneClass->_loadScene();

			delete sceneClass;
		}
		else
		{
			Worker::Loader * loader = nullptr;
			if ( _paths.empty() == false )
			{
				loader = new Worker::Loader( _paths );
			}
			else if ( _buffers.empty() == false )
			{
				loader = new Worker::Loader( _buffers );
			}
			if ( loader == nullptr )
			{
				return;
			}

			const bool trajectoryTargetsForced = _trajectoryTargets.size() > 0;

			if ( trajectoryTargetsForced )
			{
				for ( Model::Molecule * const trajectoryTarget : _trajectoryTargets )
				{
					loader->addDynamicTarget( trajectoryTarget );
				}
			}
			else
			{
				for ( const Object3D::Scene::PairMoleculePtrFloat & molPair : VTXApp::get().getScene().getMolecules() )
				{
					loader->addDynamicTarget( molPair.first );
				}
			}

			loader->setOpenTrajectoryAsMoleculeIfTargetFail( !trajectoryTargetsForced );

			VTX::Core::Worker::CallbackThread * callback = new VTX::Core::Worker::CallbackThread(
				[ loader ]( const uint p_code )
				{
					for ( const std::pair<const FilePath, Worker::Loader::Result> & pairFilResult :
						  loader->getPathsResult() )
					{
						const FilePath &			   filepath = pairFilResult.first;
						const Worker::Loader::Result & result	= pairFilResult.second;

						if ( !result.state )
							continue;

						if ( result.sourceType == Worker::Loader::SOURCE_TYPE::FILE )
						{
							if ( result.molecule != nullptr )
								VTXApp::get().getScenePathData().registerLoading( result.molecule, filepath );
							VTX::Setting::enqueueNewLoadingPath( filepath );
						}
						else if ( result.sourceType == Worker::Loader::SOURCE_TYPE::BUFFER )
						{
							VTX::Setting::enqueueNewDownloadCode( filepath.stem().string() );
						}

						if ( result.molecule != nullptr )
						{
							result.molecule->setDisplayName( filepath.stem().string() );
							VTXApp::get().getScene().addMolecule( result.molecule );
						}
						else if ( result.mesh != nullptr )
						{
							VTXApp::get().getScene().addMesh( result.mesh );
						}
					}

					VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
				} );

			VTX_THREAD( loader, callback );
		}
	}

	void OpenApi::execute() { VTX_NETWORK_REQUEST<App::Internal::Network::Request::DownloadMMTF>( _id ); }

	// TODO keep only Dialog parts here and move real loading action into VTX_APP.
	void Save::execute()
	{
		Worker::Saver * const saver = new Worker::Saver( _path );

		if ( saver == nullptr )
			return;

		VTX_THREAD( saver, _callback );

		if ( _path.extension() == "vtx" )
		{
			VTXApp::get().getScenePathData().setCurrentPath( _path, true );
			VTX_EVENT( new Event::VTXEvent( Event::Global::SCENE_SAVED ) );
		}
		else
		{
			VTX::Setting::enqueueNewLoadingPath( _path );
		}
	}

	void ImportRepresentationPreset::execute()
	{
		if ( _paths.empty() )
			return;

		for ( const FilePath & path : _paths )
		{
			FilePath target = IO::Filesystem::getRepresentationPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( std::filesystem::copy_file( path, target ) )
			{
				Worker::RepresentationLoader * const loader = new Worker::RepresentationLoader( target );
				VTX_WORKER( loader );
			}
		}
	}

	void ImportRenderEffectPreset::execute()
	{
		if ( _paths.empty() )
			return;

		for ( const FilePath & path : _paths )
		{
			FilePath target = IO::Filesystem::getRenderEffectPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( std::filesystem::copy_file( path, target ) )
			{
				Worker::RenderEffectPresetLoader * const loader = new Worker::RenderEffectPresetLoader( target );
				VTX_WORKER( loader );
			}
		}
	}

	void ToggleCamera::execute() { VTXApp::get().getScene().getCameraManager().toggle(); }

	void Snapshot::execute()
	{
		Worker::Snapshoter * worker = new Worker::Snapshoter( _mode, _path, _exportData );
		VTX_WORKER( worker );
	}

	void SetCameraProjectionToPerspective::execute()
	{
		VTXApp::get().getScene().getCameraManager().setPerspectiveCamera( !_perspective );
	}
} // namespace VTX::App::Action::Main
