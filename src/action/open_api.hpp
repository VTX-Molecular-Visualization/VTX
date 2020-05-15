#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "exception.hpp"
#include "model/molecule.hpp"
#include "open.hpp"
#include "vtx_app.hpp"
#include "worker/api_fetcher.hpp"
#include "worker/worker_manager.hpp"

namespace VTX
{
	namespace Action
	{
		class OpenApi : public BaseAction
		{
		  public:
			explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

			virtual void execute() override
			{
				Worker::ApiFetcher fetcher = Worker::ApiFetcher( API_URL_MMTF + _id );

				try
				{
					VTX_WORKER( &fetcher );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( p_e.what() );
					return;
				}

				// PathFake * path = new PathFake( _id + ".mmtf" );
				// path->write( fetcher.getBuffer() );
				// VTX_ACTION( new Action::Open( path ) );

				/*
				Worker::WorkerManager &		  manager  = VTXApp::get().getWorkerManager();
				Worker::ApiFetcher *		  fetcher  = new Worker::ApiFetcher( API_URL_MMTF + _id );
				std::string					  id	   = _id;
				std::function<void( void )> * callback = new std::function<void( void )>( [ fetcher, &id ]( void ) {
					VTX_DEBUG( "INVOKE" );
					// PathFake path = PathFake( id + ".mmtf" );
					VTX_DEBUG( "CALLBACK" );
					// path.write( fetcher->getBuffer() );
					VTX_DEBUG( "DELETE" );
					delete fetcher;
					// VTXApp::get().goToState( ID::State::LOAD, (void *)&path );
				} );

				manager.runWorker( fetcher, callback );

			*/			}

			  private:
				const std::string _id;
		};

	} // namespace Action
} // namespace VTX
#endif
