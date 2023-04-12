#include "action.hpp"
#include "dialog.hpp"
#include "id.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include <old/action/action_manager.hpp>
#include <old/action/main.hpp>
#include <old/io/struct/scene_path_data.hpp>
#include <old/object3d/scene.hpp>
#include <old/vtx_app.hpp>
#include <old/worker/loader.hpp>
#include <old/worker/saver.hpp>
#include <old/worker/scene_loader.hpp>
#include <old/worker/worker_manager.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Session::Action
{
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

			Worker::CallbackThread callback = Worker::CallbackThread(
				[ this ]( const uint p_code )
				{
					if ( p_code )
					{
						_callEffectiveOpenAction();
					}
				} );
			Dialog::leavingSessionDialog( callback );
		}
		else
		{
			_callEffectiveOpenAction();
		}
	}

	void Open::_callEffectiveOpenAction()
	{
		if ( !_trajectoryTargets.empty() )
		{
			VTX_ACTION( new VTX::Action::Main::Open( _paths[ 0 ], *_trajectoryTargets[ 0 ] ) );
		}
		else if ( !_buffers.empty() )
		{
			VTX_ACTION( new VTX::Action::Main::Open( _buffers ) );
		}
		else
		{
			VTX_ACTION( new VTX::Action::Main::Open( _paths ) );
		}
	}

	// TODO keep only Dialog parts here and move real loading action into VTX_APP.
	void Save::execute()
	{
		if ( _path.empty() )
		{
			Dialog::openSaveSessionDialog( _callback );
			return;
		}

		VTX::Action::Main::Save( _path, _callback );
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
