#include "util/network.hpp"

namespace VTX::Util::Network
{

	void httpRequestGet( const std::string & p_url, std::string & p_data )
	{
		try
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
				p_data = response.text;
			}
			else
			{
				throw HTTPException( "Protocol error: " + std::to_string( response.status_code ) );
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
		}
	}
} // namespace VTX::Util::Network
