#include "util/network.hpp"
#include "util/callback.hpp"
#include <cpr/cpr.h>

namespace
{
	cpr::SslOptions sslOpts = cpr::Ssl( cpr::ssl::MaxTLSVersion {} );

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
	void httpRequestGet(
		const std::string_view & p_url,
		std::string &			 p_text,
		const ProgressCallback & p_progress
	)
	{
		cpr::Response response = cpr::Get(
			cpr::Url { p_url },
			cpr::ProgressCallback(
				[ &p_progress ](
					cpr::cpr_off_t p_downloadTotal,
					cpr::cpr_off_t p_downloadNow,
					cpr::cpr_off_t,
					cpr::cpr_off_t,
					intptr_t ) -> bool
				{
					if ( p_progress )
					{
						return p_progress(
							static_cast<size_t>( p_downloadNow ),
							static_cast<size_t>( p_downloadTotal )
						);
					}
					return true;
				}
			),
			sslOpts
		);

		checkResponse( response );
		p_text = std::move( response.text );
	}

} // namespace VTX::Util::Network
