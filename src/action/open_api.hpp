#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#pragma warning( push, 0 )
//#include <httplib.h>
#pragma warning( pop )
#include "base_action.hpp"
#include "model/molecule.hpp"
#include "open.hpp"
#include "vtx_app.hpp"

#define CPPHTTPLIB_ZLIB_SUPPORT

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
				try
				{
					std::string url = "mmtf.rcsb.org";
					std::string body;
					VTX_DEBUG( url );
					//
					httplib::Headers headers
						= { { "Accept-Encoding", "gzip, deflate" },
							{ "Connection", "keep-alive" },
							{ "Accept",
							  "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;"
							  "q=0.8,application/signed-exchange;v=b3;q=0.9" } };
					//
					httplib::Client					   cli( url );
					std::shared_ptr<httplib::Response> response
						= cli.Get( ( "/v1.0/full/" + _id ).c_str(),
								   headers,
								   /*
								   [ & ]( const httplib::Response & p_response ) {
									   VTX_DEBUG( "HANDLER" );
									   VTX_DEBUG( std::to_string( p_response.status ) );
									   VTX_DEBUG( std::to_string( p_response.content_length ) );
									   VTX_DEBUG( p_response.body );
									   VTX_DEBUG( body );
									   return true;
								   },
								   */
								   [ & ]( const char * data, uint64_t len ) {
									   VTX_DEBUG( "DATA" );
									   body.append( data, len );
									   VTX_DEBUG( data );
									   VTX_DEBUG( std::to_string( len ) );
									   return true;
								   },
								   []( uint64_t len, uint64_t total ) {
									   VTX_DEBUG( "PROGRESS" );
									   VTX_DEBUG( std::to_string( (int)( ( len / total ) * 100 ) ) + "%" );
									   return true; // return 'false' if you want to cancel the request.
								   } );

					if ( response == nullptr ) { VTX_DEBUG( "nullptr" ); }
					else
					{
						VTX_DEBUG( "OK" );
						VTX_DEBUG( std::to_string( response->status ) );
						VTX_DEBUG( response->body );
					}
					VTX_DEBUG( body );
					// VTXApp::get().goToState( ID::State::LOAD, (void *)&_id );
				}
				catch ( const std::exception p_e )
				{
					VTX_ERROR( p_e.what() );
				}
			};

		  private:
			const std::string & _id;
		}; // namespace Action
	}	   // namespace Action
} // namespace VTX
#endif
