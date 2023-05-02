#include "app/action/main.hpp"
#include "app/application/scene.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/internal/network/request/download_mmtf.hpp"
#include "app/internal/scene/camera_manager.hpp"
#include "app/internal/worker/loader.hpp"
#include "app/internal/worker/render_effect_loader.hpp"
#include "app/internal/worker/representation_loader.hpp"
#include "app/internal/worker/saver.hpp"
#include "app/internal/worker/scene_loader.hpp"
#include "app/network.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker.hpp"

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
		Internal::Worker::SceneLoader * sceneLoader = new Internal::Worker::SceneLoader( _paths );
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
			Internal::Worker::Loader * loader = nullptr;
			if ( _paths.empty() == false )
			{
				loader = new Internal::Worker::Loader( _paths );
			}
			else if ( _buffers.empty() == false )
			{
				loader = new Internal::Worker::Loader( _buffers );
			}
			if ( loader == nullptr )
			{
				return;
			}

			const bool trajectoryTargetsForced = _trajectoryTargets.size() > 0;

			if ( trajectoryTargetsForced )
			{
				for ( App::Component::Chemistry::Molecule * const trajectoryTarget : _trajectoryTargets )
				{
					loader->addDynamicTarget( trajectoryTarget );
				}
			}
			else
			{
				for ( const App::Application::Scene::PairMoleculePtrFloat & molPair :
					  VTXApp::get().getScene().getMolecules() )
				{
					loader->addDynamicTarget( molPair.first );
				}
			}

			loader->setOpenTrajectoryAsMoleculeIfTargetFail( !trajectoryTargetsForced );

			VTX::App::Core::Worker::CallbackThread * callback = new VTX::App::Core::Worker::CallbackThread(
				[ loader ]( const uint p_code )
				{
					for ( const std::pair<const FilePath, Internal::Worker::Loader::Result> & pairFilResult :
						  loader->getPathsResult() )
					{
						const FilePath &						 filepath = pairFilResult.first;
						const Internal::Worker::Loader::Result & result	  = pairFilResult.second;

						if ( !result.state )
							continue;

						if ( result.sourceType == Internal::Worker::Loader::SOURCE_TYPE::FILE )
						{
							if ( result.molecule != nullptr )
								VTXApp::get().getScenePathData().registerLoading( result.molecule, filepath );
							VTX::App::Application::Setting::enqueueNewLoadingPath( filepath );
						}
						else if ( result.sourceType == Internal::Worker::Loader::SOURCE_TYPE::BUFFER )
						{
							VTX::App::Application::Setting::enqueueNewDownloadCode( filepath.stem().string() );
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
		Internal::Worker::Saver * const saver = new Internal::Worker::Saver( _path );

		if ( saver == nullptr )
			return;

		VTX_THREAD( saver, _callback );

		if ( _path.extension() == "vtx" )
		{
			VTXApp::get().getScenePathData().setCurrentPath( _path, true );
			VTX_EVENT( VTX::App::Event::Global::SCENE_SAVED );
		}
		else
		{
			VTX::App::Application::Setting::enqueueNewLoadingPath( _path );
		}
	}

	void ImportRepresentationPreset::execute()
	{
		if ( _paths.empty() )
			return;

		for ( const FilePath & path : _paths )
		{
			FilePath target = App::Internal::IO::Filesystem::getRepresentationPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( std::filesystem::copy_file( path, target ) )
			{
				Internal::Worker::RepresentationLoader * const loader
					= new Internal::Worker::RepresentationLoader( target );
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
			FilePath target = App::Internal::IO::Filesystem::getRenderEffectPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( std::filesystem::copy_file( path, target ) )
			{
				Internal::Worker::RenderEffectPresetLoader * const loader
					= new Internal::Worker::RenderEffectPresetLoader( target );
				VTX_WORKER( loader );
			}
		}
	}

	void ToggleCamera::execute() { VTXApp::get().getScene().getCameraManager().toggle(); }

	void Snapshot::execute()
	{
		Render::Worker::Snapshoter * worker = new Render::Worker::Snapshoter( _mode, _path, _exportData );
		VTX_WORKER( worker );
	}

	void SetCameraProjectionToPerspective::execute()
	{
		VTXApp::get().getScene().getCameraManager().setPerspectiveCamera( !_perspective );
	}
} // namespace VTX::App::Action::Main
