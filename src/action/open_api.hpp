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
				Worker::ApiFetcher * const fetcher = new Worker::ApiFetcher( API_URL_MMTF + _id );

				std::string					  id	   = _id;
				Path *						  path	   = new Path( id + ".mmtf" );
				std::function<void( void )> * callback = new std::function<void( void )>( [ fetcher, path ]( void ) {
					std::map<Path *, std::string *> mapBuffers = std::map<Path *, std::string *>();
					mapBuffers.emplace( path, fetcher->getBuffer() );
					// TODO: another thread here to load the file.
					VTX_WORKER( new Worker::Loader( mapBuffers ) );
					delete fetcher;
				} );

				VTX_WORKER( fetcher, callback );
			}

		  private:
			const std::string _id;
		};

	} // namespace Action
} // namespace VTX
#endif
