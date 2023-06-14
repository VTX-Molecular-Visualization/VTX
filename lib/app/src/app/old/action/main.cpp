#include "app/old/action/main.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/event.hpp"
#include "app/old/event/global.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/internal/network/request/download_mmtf.hpp"
#include "app/old/internal/scene/camera_manager.hpp"
#include "app/old/internal/worker/loader.hpp"
#include "app/old/internal/worker/render_effect_loader.hpp"
#include "app/old/internal/worker/representation_loader.hpp"
#include "app/old/internal/worker/saver.hpp"
#include "app/old/internal/worker/scene_loader.hpp"
#include "app/old/network.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/worker.hpp"

namespace VTX::App::Old::Action::Main
{
	void New::execute()
	{
		App::Old::VTXApp::get().getScene().reset();
		App::Old::VTXApp::get().getScenePathData().clearCurrentPath();
	}
	// TODO keep only Dialog parts here and move real loading action into VTX_APP.
	void Open::LoadSceneClass::_loadScene()
	{
		Internal::Worker::SceneLoader * sceneLoader = new Internal::Worker::SceneLoader( _paths );
		VTX_WORKER( sceneLoader );

		for ( const FilePath & path : _paths )
		{
			App::Old::VTXApp::get().getScenePathData().setCurrentPath( path, true );
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

			App::Old::VTXApp::get().getScene().clear();

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
				for ( App::Old::Component::Chemistry::Molecule * const trajectoryTarget : _trajectoryTargets )
				{
					loader->addDynamicTarget( trajectoryTarget );
				}
			}
			else
			{
				for ( const App::Old::Application::Scene::PairMoleculePtrFloat & molPair :
					  App::Old::VTXApp::get().getScene().getMolecules() )
				{
					loader->addDynamicTarget( molPair.first );
				}
			}

			loader->setOpenTrajectoryAsMoleculeIfTargetFail( !trajectoryTargetsForced );

			VTX::App::Old::Core::Worker::CallbackThread * callback = new VTX::App::Old::Core::Worker::CallbackThread(
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
								App::Old::VTXApp::get().getScenePathData().registerLoading( result.molecule, filepath );
							VTX::App::Old::Application::Setting::enqueueNewLoadingPath( filepath );
						}
						else if ( result.sourceType == Internal::Worker::Loader::SOURCE_TYPE::BUFFER )
						{
							VTX::App::Old::Application::Setting::enqueueNewDownloadCode( filepath.stem().string() );
						}

						if ( result.molecule != nullptr )
						{
							result.molecule->setDisplayName( filepath.stem().string() );
							App::Old::VTXApp::get().getScene().addMolecule( result.molecule );
						}
						else if ( result.mesh != nullptr )
						{
							App::Old::VTXApp::get().getScene().addMesh( result.mesh );
						}
					}

					App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
				} );

			VTX_THREAD( loader, callback );
		}
	}

	void OpenApi::execute() { VTX_NETWORK_REQUEST<App::Old::Internal::Network::Request::DownloadMMTF>( _id ); }

	// TODO keep only Dialog parts here and move real loading action into VTX_APP.
	void Save::execute()
	{
		Internal::Worker::Saver * const saver = new Internal::Worker::Saver( _path );

		if ( saver == nullptr )
			return;

		VTX_THREAD( saver, _callback );

		if ( _path.extension() == "vtx" )
		{
			App::Old::VTXApp::get().getScenePathData().setCurrentPath( _path, true );
			VTX_EVENT( VTX::App::Old::Event::Global::SCENE_SAVED );
		}
		else
		{
			VTX::App::Old::Application::Setting::enqueueNewLoadingPath( _path );
		}
	}

	void ImportRepresentationPreset::execute()
	{
		if ( _paths.empty() )
			return;

		for ( const FilePath & path : _paths )
		{
			FilePath target = App::Old::Internal::IO::Filesystem::getRepresentationPath( path.filename() );
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
			FilePath target = App::Old::Internal::IO::Filesystem::getRenderEffectPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( std::filesystem::copy_file( path, target ) )
			{
				Internal::Worker::RenderEffectPresetLoader * const loader
					= new Internal::Worker::RenderEffectPresetLoader( target );
				VTX_WORKER( loader );
			}
		}
	}

	void ToggleCamera::execute() { App::Old::VTXApp::get().getScene().getCameraManager().toggle(); }

	void Snapshot::execute()
	{
		Render::Worker::Snapshoter * worker = new Render::Worker::Snapshoter( _mode, _path, _exportData );
		VTX_WORKER( worker );
	}

	void SetCameraProjectionToPerspective::execute()
	{
		App::Old::VTXApp::get().getScene().getCameraManager().setPerspectiveCamera( !_perspective );
	}
} // namespace VTX::App::Old::Action::Main
