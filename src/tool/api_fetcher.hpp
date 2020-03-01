#ifndef __VTX_API_FETCHER__
#define __VTX_API_FETCHER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_runnable.hpp"
#include "vtx_app.hpp"
#include <curl/curl.h>

namespace VTX
{
	namespace Tool
	{
		class ApiFetcher : public Generic::BaseRunnable
		{
		  public:
			explicit ApiFetcher( std::string & p_url ) : _url( p_url ) {}

			std::string & getBuffer() { return _buffer; }

		  protected:
			virtual void _run() override;

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
				VTX_DEBUG( std::to_string( progress * 100 ) + "%" );

				VTXApp::get().getEventManager().fireEvent( Event::Global::UPDATE_PROGRESS_BAR, &progress );

				return 0;
			}
		};
	} // namespace Tool
} // namespace VTX
#endif
