#include "api_fetcher.hpp"
#include "exception.hpp"

namespace VTX
{
	namespace Tool
	{
		// THROW EXCEPTIONS.
		void ApiFetcher::_run()
		{
			std::string url = _url;
			CURL *		curl;
			CURLcode	result;
			curl_global_init( CURL_GLOBAL_DEFAULT );
			curl = curl_easy_init();
			if ( curl )
			{
				curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
				curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, ApiFetcher::_writeCallback );
				curl_easy_setopt( curl, CURLOPT_WRITEDATA, &_buffer );
				curl_easy_setopt( curl, CURLOPT_NOPROGRESS, FALSE );
				curl_easy_setopt( curl, CURLOPT_XFERINFOFUNCTION, ApiFetcher::_progressCallback );
				curl_easy_setopt( curl, CURLOPT_ACCEPT_ENCODING, "gzip" );

				result = curl_easy_perform( curl );
				if ( result == CURLE_OK )
				{
					long code;
					curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &code );

					if ( code == 200 )
					{
						if ( _buffer.empty() ) { throw Exception::VTXException( "Empty buffer" ); }
					}
					else
					{
						throw Exception::VTXException( "Protocol error: " + std::to_string( code ) );
					}
				}
				else
				{
					throw Exception::VTXException( "cURL failed: " + std::to_string( result ) );
				}
				curl_easy_cleanup( curl );
			}
			else
			{
				throw Exception::VTXException( "cURL failed" );
			}

			curl_global_cleanup();
		}

	} // namespace Tool
} // namespace VTX
