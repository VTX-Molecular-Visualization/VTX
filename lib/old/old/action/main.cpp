#include "main.hpp"
#include "action/action_manager.hpp"
#include "controller/measurement_picker.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/struct/scene_path_data.hpp"
#include "mvc/mvc_manager.hpp"
#include "network/network_manager.hpp"
#include "network/request/check_update.hpp"
#include "network/request/download_mmtf.hpp"
#include "object3d/scene.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "util/molecule.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/representation_loader.hpp"
#include "worker/saver.hpp"
#include "worker/scene_loader.hpp"

namespace VTX::Action::Main
{
	void New::execute()
	{
		VTXApp::get().getScene().reset();
		VTXApp::get().getScenePathData().clearCurrentPath();
	}

	void Quit::execute() { VTXApp::get().closeAllWindows(); };

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
			UI::Dialog::leavingSessionDialog( callback );
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
	void Open::LoadSceneClass::_loadScene()
	{
		Worker::SceneLoader * sceneLoader = new Worker::SceneLoader( _paths );
		VTX_WORKER( sceneLoader );

		for ( const Util::FilePath & path : _paths )
		{
			VTXApp::get().getScenePathData().setCurrentPath( path, true );
		}
	}

	void OpenApi::execute() { VTX_NETWORK_MANAGER().sendRequest( new Network::Request::DownloadMMTF( _id ) ); }

	void Save::execute()
	{
		if ( _path.empty() )
		{
			UI::Dialog::openSaveSessionDialog( _callback );
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
			VTX::Setting::enqueueNewLoadingPath( _path );
	}

	void ImportRepresentationPreset::execute()
	{
		if ( _paths.empty() )
			return;

		for ( const Util::FilePath & path : _paths )
		{
			Util::FilePath target = Util::Filesystem::getRepresentationPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( Util::Filesystem::copyFile( path, target ) )
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

		for ( const Util::FilePath & path : _paths )
		{
			Util::FilePath target = Util::Filesystem::getRenderEffectPath( path.filename() );
			Util::Filesystem::generateUniqueFileName( target );
			if ( Util::Filesystem::copyFile( path, target ) )
			{
				Worker::RenderEffectPresetLoader * const loader = new Worker::RenderEffectPresetLoader( target );
				VTX_WORKER( loader );
			}
		}
	}

	void ToggleCameraController::execute()
	{
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->toggleCameraController();
	};

	void ChangeCameraController::execute()
	{
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->setCameraController( _id );
	};

	void ResetCameraController::execute()
	{
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->resetCameraController();
	};

	void ChangePicker::execute()
	{
		State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != _pickerController )
			state->setPickerController( _pickerController );

		if ( _mode > -1 )
		{
			if ( _pickerController == ID::Controller::MEASUREMENT )
			{
				Controller::MeasurementPicker * const measurementController
					= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

				measurementController->setCurrentMode( Controller::MeasurementPicker::Mode( _mode ) );
			}
		}
	};

	Snapshot::Snapshot( const Worker::Snapshoter::MODE p_mode, const Util::FilePath & p_path ) :
		Snapshot( p_mode,
				  p_path,
				  IO::Struct::ImageExport( IO::Struct::ImageExport::RESOLUTION ::Free,
										   VTX_SETTING().getSnapshotBackgroundOpacity(),
										   VTX_SETTING().getSnapshotQuality() ) )
	{
	}
	Snapshot::Snapshot( const Worker::Snapshoter::MODE			  p_mode,
						const Util::FilePath &					  p_path,
						const IO::Struct::ImageExport::RESOLUTION p_resolution ) :
		Snapshot( p_mode,
				  p_path,
				  IO::Struct::ImageExport( p_resolution,
										   VTX_SETTING().getSnapshotBackgroundOpacity(),
										   VTX_SETTING().getSnapshotQuality() ) )
	{
	}
	Snapshot::Snapshot( const Worker::Snapshoter::MODE p_mode,
						const Util::FilePath &		   p_path,
						const int					   p_width,
						const int					   p_height ) :
		Snapshot( p_mode,
				  p_path,
				  IO::Struct::ImageExport( std::pair<const int, int>( p_width, p_height ),
										   VTX_SETTING().getSnapshotBackgroundOpacity(),
										   VTX_SETTING().getSnapshotQuality() ) )
	{
	}
	Snapshot::Snapshot( const Worker::Snapshoter::MODE	p_mode,
						const Util::FilePath &			p_path,
						const IO::Struct::ImageExport & p_exportData ) :
		_mode( p_mode ),
		_path( p_path ), _exportData( p_exportData )
	{
	}

	void Snapshot::execute()
	{
		Worker::Snapshoter * worker = new Worker::Snapshoter( _mode, _path, _exportData );
		VTX_WORKER( worker );
	};

	void ClearConsoleInterface::execute() { VTX_EVENT( new Event::VTXEvent( Event::Global::CLEAR_CONSOLE ) ); };

	void RestoreWindowLayout::execute() { VTXApp::get().getMainWindow().restoreDefaultLayout(); };

	void CheckForUpdate::execute()
	{
		VTX_NETWORK_MANAGER().sendRequest( new Network::Request::CheckUpdate( _showPopupIfNoUpdate ) );
	};

} // namespace VTX::Action::Main
