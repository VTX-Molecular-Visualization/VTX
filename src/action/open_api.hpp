#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "io/path_fake.hpp"
#include "model/molecule.hpp"
#include "open.hpp"
#include "vtx_app.hpp"
#include <curl/curl.h>

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
					std::string url = API_URL_MMTF + _id;
					VTX_DEBUG( url );
					CURL *		curl;
					CURLcode	result;
					std::string buffer;
					curl_global_init( CURL_GLOBAL_DEFAULT );
					curl = curl_easy_init();
					if ( curl )
					{
						curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
						curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, OpenApi::_writeCallback );
						curl_easy_setopt( curl, CURLOPT_WRITEDATA, &buffer );
						curl_easy_setopt( curl, CURLOPT_ACCEPT_ENCODING, "gzip" );
						result = curl_easy_perform( curl );
						if ( result == CURLE_OK )
						{
							long code;
							curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &code );

							if ( code == 200 )
							{
								IO::PathFake path = IO::PathFake( _id + ".mmtf" );
								path.write( buffer );
								VTXApp::get().goToState( ID::State::LOAD, (void *)&path );
							}
							else
							{
								VTX_ERROR( "Failed to download file" );
							}
						}
						else
						{
							VTX_ERROR( "Failed to download file" );
						}
						curl_easy_cleanup( curl );
					}
					else
					{
						VTX_ERROR( "Failed to download file" );
					}
					curl_global_cleanup();
				}
				catch ( const std::exception p_e )
				{
					VTX_ERROR( p_e.what() );
				}
			};

		  private:
			const std::string & _id;

			static size_t _writeCallback( void * p_content, size_t p_size, size_t p_nmemb, void * p_userp )
			{
				( (std::string *)p_userp )->append( (char *)p_content, p_size * p_nmemb );
				return p_size * p_nmemb;
			}

		}; // namespace Action
	}	   // namespace Action
} // namespace VTX
#endif
