#ifndef __VTX_ACTION_NEW__
#define __VTX_ACTION_NEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "define.hpp"
#include "io/reader/vtx.hpp"
#include "io/writer/vtx.hpp"
#include "state/visualization.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/api_fetcher.hpp"
#include "worker/snapshoter.hpp"

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
					VTXApp::get().getScene().clean();
					Model::Path * path = Generic::create<Model::Path>();
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
						std::map<Path *, std::string *> mapBuffers = std::map<Path *, std::string *>();
						mapBuffers.emplace( path, fetcher->getBuffer() );
						delete fetcher;

						// Open the buffer.
						VTX_ACTION( new Open( mapBuffers ) );
					} );

					const Worker::CallbackError * error
						= new Worker::CallbackError( [ fetcher ]( const std::exception & p_e ) {
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
				explicit Save() {}
				virtual void execute() override
				{
					IO::Writer::VTX writer = IO::Writer::VTX();
					try
					{
						writer.writeFile( Util::Filesystem::VTX_JSON_FILE, VTXApp::get() );
						VTX_INFO( "App saved " );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Cannot save app: " + std::string( p_e.what() ) );
					}
				}
			};

			class ChangeCameraController : public BaseAction
			{
			  public:
				explicit ChangeCameraController() {}

				virtual void execute() override
				{
					VTXApp::get()
						.getStateMachine()
						.getItem<State::Visualization>( ID::State::VISUALIZATION )
						->toggleController();
				};

			  private:
			};

			class Snapshot : public BaseAction
			{
			  public:
				explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const Path & p_path ) :
					_mode( p_mode ), _path( p_path )
				{
				}

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

			class Resize : public BaseAction
			{
			  public:
				Resize( const uint p_width, const uint p_height ) : _width( p_width ), _height( p_height ) {}
				virtual void execute() override
				{
					// Set camera.
					VTXApp::get().getScene().getCamera().setScreenSize( _width, _height );

					// Resize renderer.
					VTXApp::get().getRendererGL().resize( _width, _height );
				};

			  private:
				uint _width;
				uint _height;
			};

			class ActiveUIComponent : public BaseAction
			{
			  public:
				explicit ActiveUIComponent( const std::string & p_name, const bool p_active ) :
					_name( p_name ), _active( p_active )
				{
				}

				virtual void execute() override
				{
					VTXApp::get().getUI().getComponentByName( _name )->setVisible( _active );
				};

			  private:
				const std::string & _name;
				const bool			_active;
			};

		} // namespace Main
	}	  // namespace Action
} // namespace VTX
#endif
