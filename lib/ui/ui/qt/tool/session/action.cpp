#include "action.hpp"
#include "dialog.hpp"
#include "id.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include <old/io/struct/scene_path_data.hpp>
#include <old/object3d/scene.hpp>
#include <old/tool/logger.hpp>
#include <old/vtx_app.hpp>
#include <old/worker/loader.hpp>
#include <old/worker/saver.hpp>
#include <old/worker/scene_loader.hpp>
#include <old/worker/worker_manager.hpp>

namespace VTX::UI::QT::Tool::Session::Action
{
	void Quit::execute() { QT_APP()->closeAllWindows(); }

	void Open::LoadSceneClass::_loadScene()
	{
		Worker::SceneLoader * sceneLoader = new Worker::SceneLoader( _paths );
		VTX_WORKER( sceneLoader );

		for ( const Util::FilePath & path : _paths )
		{
			VTXApp::get().getScenePathData().setCurrentPath( path, true );
		}
	}

	void Open::execute()
	{
		bool loadScene = false;
		for ( const Util::FilePath & path : _paths )
		{
			loadScene = loadScene || path.extension() == "vtx";
		}

		if ( loadScene )
		{
			if ( _paths.empty() )
				return;

			LoadSceneClass * const sceneClass = new LoadSceneClass( _paths );

			Worker::CallbackThread callback = Worker::CallbackThread(
				[ sceneClass ]( const uint p_code )
				{
					if ( p_code )
					{
						VTXApp::get().getScene().clear();
						sceneClass->_loadScene();
					}

					delete sceneClass;
				} );
			Dialog::leavingSessionDialog( callback );
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

			Worker::CallbackThread * callback = new Worker::CallbackThread(
				[ loader ]( const uint p_code )
				{
					for ( const std::pair<const Util::FilePath, Worker::Loader::Result> & pairFilResult :
						  loader->getPathsResult() )
					{
						const Util::FilePath &		   filepath = pairFilResult.first;
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
							VTX::Setting::enqueueNewDownloadCode( filepath.filenameWithoutExtension() );
						}

						if ( result.molecule != nullptr )
						{
							result.molecule->setDisplayName( filepath.filenameWithoutExtension() );
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

	void Save::execute()
	{
		if ( _path.empty() )
		{
			Dialog::openSaveSessionDialog( _callback );
			return;
		}

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

	void ToggleCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->toggleCameraController();
	}

	void ChangeCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->setCameraController( _id );
	};

	void ResetCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->resetCameraController();
	};
} // namespace VTX::UI::QT::Tool::Session::Action
