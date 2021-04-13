#ifndef __VTX_ACTION_MAIN__
#define __VTX_ACTION_MAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "base_action.hpp"
#include "define.hpp"
#include "io/reader/vtx.hpp"
#include "io/writer/vtx.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include "network/network_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include "worker/saver.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::Action::Main
{
	class New : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class Quit : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().stop(); };
	};

	class Open : public BaseAction
	{
	  public:
		explicit Open( FilePath * const p_path ) { _paths.emplace_back( p_path ); }
		explicit Open( const std::vector<FilePath *> & p_paths ) : _paths( p_paths ) {}
		explicit Open( const std::map<FilePath *, std::string *> & p_buffers ) : _buffers( p_buffers ) {}

		virtual void execute() override
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

			/*
			const Worker::CallbackSuccess * success
				= new Worker::CallbackSuccess( [ loader ]( void ) { delete loader; } );
			const Worker::CallbackError * error
				= new Worker::CallbackError( [ loader ]( const std::exception & p_e ) {
					  VTX_ERROR( p_e.what() );
					  delete loader;
				  } );

			VTX_WORKER( loader, success, error );
			*/

			VTX_WORKER( loader );
			delete loader;
		}

	  private:
		std::vector<FilePath *>				_paths = std::vector<FilePath *>();
		std::map<FilePath *, std::string *> _buffers;
	};

	class OpenApi : public BaseAction
	{
	  public:
		explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

		void execute() override { VTX_NETWORK_MANAGER().downloadMMTF( _id ); }

	  private:
		const std::string _id;
	};

	class Save : public BaseAction
	{
	  public:
		explicit Save( FilePath * p_path ) : _path( p_path ) {}

		virtual void execute() override
		{
			Worker::Saver * saver = new Worker::Saver( _path );
			VTX_WORKER( saver );
			delete saver;
		}

	  private:
		FilePath * _path;
	};

	class ResetScene : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class ToggleCameraController : public BaseAction
	{
	  public:
		explicit ToggleCameraController() {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->toggleCameraController();
		};
	};

	class ChangeCameraController : public BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->setCameraController( _id );
		};

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public BaseAction
	{
	  public:
		explicit ResetCameraController() {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->resetCameraController();
		};

	  private:
	};

	class Snapshot : public BaseAction
	{
	  public:
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const FilePath & p_path ) :
			_mode( p_mode ), _path( p_path )
		{
		}

		virtual void execute() override
		{
			Worker::Snapshoter * const worker = new Worker::Snapshoter(
				_mode, _path, VTXApp::get().getMainWindow().getOpenGLWidget().grabFramebuffer() );
			VTX_WORKER( worker );
		};

		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE _mode;
		const FilePath				   _path;
	};

} // namespace VTX::Action::Main
#endif
