#ifndef __VTX_UTIL_NETWORK__
#define __VTX_UTIL_NETWORK__

#include "exceptions.hpp"
#include "logger.hpp"
#include <cpr/cpr.h>

namespace VTX::Util::Network
{

	void httpRequestGet( const std::string & p_url, std::string & p_data )
	{
		try
		{
			cpr::Response response { cpr::Get( p_url ) };

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

#endif
