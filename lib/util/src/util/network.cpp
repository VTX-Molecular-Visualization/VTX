#include "util/network.hpp"
#include <cpr/cpr.h>

namespace VTX::Util::Network
{

	void httpRequestGet( const std::string_view & p_url, std::string * const p_data )
	{
		cpr::Response response = cpr::Get(
			cpr::Url { p_url },
			cpr::ProgressCallback(
				[ & ](
					cpr::cpr_off_t downloadTotal,
					cpr::cpr_off_t downloadNow,
					cpr::cpr_off_t uploadTotal,
					cpr::cpr_off_t uploadNow,
					intptr_t	   userdata
				) -> bool
				{
					VTX_DEBUG( "Downloaded {} / {} bytes", downloadNow, downloadTotal );
					return true;
				}
			)
		);

		if ( response.status_code == 200 )
		{
			*p_data = response.text;
		}
		else
		{
			throw HTTPException( "Protocol error: " + std::to_string( response.status_code ) );
		}
	}
} // namespace VTX::Util::Network
