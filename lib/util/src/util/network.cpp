#include "util/network.hpp"
#include "util/callback.hpp"
#include <cpr/cpr.h>

namespace
{
	cpr::SslOptions sslOpts = cpr::Ssl( cpr::ssl::MaxTLSVersion {} );

	cpr::ProgressCallback progressCallback = cpr::ProgressCallback(
		[]( cpr::cpr_off_t downloadTotal,
			cpr::cpr_off_t downloadNow,
			cpr::cpr_off_t uploadTotal,
			cpr::cpr_off_t uploadNow,
			intptr_t	   userdata ) -> bool
		{
			VTX::VTX_DEBUG( "Downloaded {} / {} bytes", downloadNow, downloadTotal );
			return true;
		}
	);

	void checkResponse( cpr::Response & p_response )
	{
		if ( p_response.status_code != 200 )
		{
			throw VTX::HTTPException( p_response.error.message );
		}
	}

} // namespace

namespace VTX::Util::Network
{
	void httpRequestGet( const std::string_view & p_url, std::string & p_text )
	{
		cpr::Response response = cpr::Get( cpr::Url { p_url }, progressCallback, sslOpts );
		checkResponse( response );
		p_text = response.text;
	}

	void httpRequestGetAsync( const std::string_view & p_url, const CallbackHttpGet & p_callback )
	{
		cpr::AsyncWrapper async = cpr::GetCallback(
			[ p_callback ]( cpr::Response p_response )
			{
				checkResponse( p_response );
				p_callback( p_response.text );
			},
			cpr::Url { p_url },
			progressCallback,
			sslOpts
		);
	}
} // namespace VTX::Util::Network
