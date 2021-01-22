#ifndef __VTX_ACTION_NEW__
#define __VTX_ACTION_NEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "base_action.hpp"
#include "define.hpp"
#include "io/reader/vtx.hpp"
#include "io/writer/vtx.hpp"
#include "mvc/mvc_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/api_fetcher.hpp"
#include "worker/loader.hpp"
#include "worker/saver.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Main
		{
			class New : public BaseAction
			{
			  public:
				virtual void execute() override
				{
					VTXApp::get().getScene().clear();
					Model::Path * path = MVC::MvcManager::get().instantiateModel<Model::Path>();
					VTXApp::get().getScene().addPath( path );
				}
			};

			class Quit : public BaseAction
			{
			  public:
				virtual void execute() override { VTXApp::get().stop(); };
			};

			class Open : public BaseAction
			{
			  public:
				explicit Open( Path * p_path ) { _paths.emplace_back( p_path ); }
				explicit Open( const std::vector<Path *> & p_paths ) : _paths( p_paths ) {}
				explicit Open( const std::map<Path *, std::string *> & p_buffers ) : _buffers( p_buffers ) {}

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
				std::vector<Path *>				_paths = std::vector<Path *>();
				std::map<Path *, std::string *> _buffers;
			};

			class OpenApi : public BaseAction
			{
			  public:
				explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

				virtual void execute() override
				{
					Worker::ApiFetcher * const fetcher = new Worker::ApiFetcher( API_URL_MMTF + _id );

					std::string id	 = _id;
					Path *		path = new Path( id + ".mmtf" );

					const Worker::CallbackSuccess * success = new Worker::CallbackSuccess( [ fetcher, path ]( void ) {
						std::map<Path *, std::string *> & mapBuffers = std::map<Path *, std::string *>();
						mapBuffers.emplace( path, fetcher->getBuffer() );
						delete fetcher;

						// Open the buffer.
						VTX_ACTION( new Open( mapBuffers ) );
					} );

					const Worker::CallbackError * error = new Worker::CallbackError( [ fetcher ]( const std::exception & p_e ) {
						VTX_ERROR( p_e.what() );
						delete fetcher->getBuffer();
						delete fetcher;
					} );

					VTX_WORKER( fetcher, success, error );
				}

			  private:
				const std::string _id;
			};

			class Save : public BaseAction
			{
			  public:
				explicit Save( Path * p_path ) : _path( p_path ) {}

				virtual void execute() override
				{
					Worker::Saver * saver = new Worker::Saver( _path );
					VTX_WORKER( saver );
					delete saver;
				}

			  private:
				Path * _path;
			};

			class ToggleCameraController : public BaseAction
			{
			  public:
				explicit ToggleCameraController() {}

				virtual void execute() override { VTXApp::get().getStateMachine().getItem<State::Visualization>( ID::State::VISUALIZATION )->toggleCameraController(); };
			};

			class ChangeCameraController : public BaseAction
			{
			  public:
				explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}

				virtual void execute() override { VTXApp::get().getStateMachine().getItem<State::Visualization>( ID::State::VISUALIZATION )->setCameraController( _id ); };

			  private:
				const ID::VTX_ID _id;
			};

			class ResetCameraController : public BaseAction
			{
			  public:
				explicit ResetCameraController() {}

				virtual void execute() override { VTXApp::get().getStateMachine().getItem<State::Visualization>( ID::State::VISUALIZATION )->resetCameraController(); };

			  private:
			};

			class Snapshot : public BaseAction
			{
			  public:
				explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const Path & p_path ) : _mode( p_mode ), _path( p_path ) {}

				virtual void execute() override
				{
					Worker::Snapshoter snapshoter;

					if ( _mode == Worker::Snapshoter::MODE::GL && snapshoter.takeSnapshotGL( _path ) )
					{
						VTX_INFO( "Snapshot taken: " + _path.filename().string() );
					}
					else if ( _mode == Worker::Snapshoter::MODE::RT_CPU && snapshoter.takeSnapshotRTCPU( _path ) )
					{
						VTX_INFO( "Render computed: " + _path.filename().string() );
					}
					else if ( _mode == Worker::Snapshoter::MODE::RT_OPTIX && snapshoter.takeSnapshotRTOptix( _path ) )
					{
						VTX_INFO( "Render computed: " + _path.filename().string() );
					}
					else
					{
						VTX_WARNING( "Failed: " + _path.string() );
					}
				};

				virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

			  private:
				const Worker::Snapshoter::MODE _mode;
				const Path					   _path;
			};
		} // namespace Main
	}	  // namespace Action
} // namespace VTX
#endif
