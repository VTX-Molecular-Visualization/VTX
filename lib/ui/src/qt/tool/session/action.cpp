#include "ui/qt/tool/session/action.hpp"
#include "ui/id.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include "ui/qt/tool/session/dialog.hpp"
#include <app/action/main.hpp>
#include <app/internal/worker/loader.hpp>
#include <app/internal/worker/saver.hpp>
#include <app/internal/worker/scene_loader.hpp>
#include <app/old_app/id.hpp>
#include <app/internal/io/serialization/scene_path_data.hpp>
#include <app/application/scene.hpp>
#include <app/old_app/vtx_app.hpp>
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

			VTX::App::Core::Worker::CallbackThread callback = VTX::App::Core::Worker::CallbackThread(
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
			VTX_ACTION( new VTX::App::Action::Main::Open( _paths[ 0 ], *_trajectoryTargets[ 0 ] ) );
		}
		else if ( !_buffers.empty() )
		{
			VTX_ACTION( new VTX::App::Action::Main::Open( _buffers ) );
		}
		else
		{
			VTX_ACTION( new VTX::App::Action::Main::Open( _paths ) );
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

		VTX_ACTION( new VTX::App::Action::Main::Save( _path, _callback ) );
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
