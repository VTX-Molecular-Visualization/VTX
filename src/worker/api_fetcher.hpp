#ifndef __VTX_API_FETCHER__
#define __VTX_API_FETCHER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include "vtx_app.hpp"
#include <curl/curl.h>

namespace VTX
{
	namespace Worker
	{
		class ApiFetcher : public Worker::BaseWorker
		{
		  public:
			explicit ApiFetcher( std::string & p_url ) : _url( p_url ) {}

			std::string & getBuffer() { return _buffer; }
			virtual void  work() override;

		  private:
			std::string _url;
			std::string _buffer;

			static size_t _writeCallback( void * p_content, size_t p_size, size_t p_nmemb, void * p_userp )
			{
				( (std::string *)p_userp )->append( (char *)p_content, p_size * p_nmemb );
				return p_size * p_nmemb;
			}

			static int _progressCallback( void *	 p_clientp,
										  curl_off_t p_dltotal,
										  curl_off_t p_dlnow,
										  curl_off_t p_ultotal,
										  curl_off_t p_ulnow )
			{
				float progress = 0.f;
				if ( p_dltotal > 0.f ) { progress = (float)p_dlnow / (float)p_dltotal; }

				VTXApp::get().getEventManager().fireEvent(
					new Event::VTXEventFloat( Event::Global::UPDATE_PROGRESS_BAR, progress ) );

				return 0;
			}
		};
	} // namespace Worker
} // namespace VTX
#endif
