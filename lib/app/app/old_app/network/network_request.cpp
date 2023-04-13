#include "network_request.hpp"
#include <util/logger.hpp>

namespace VTX
{
	namespace Network
	{
		void NetworkRequest::_finished( NetworkReply * const p_reply )
		{
			_validateReply( p_reply ) ? _success( p_reply ) : _error( p_reply );
		}

		bool NetworkRequest::_validateReply( NetworkReply * const p_reply )
		{
			if ( p_reply->error() )
			{
				VTX_ERROR( p_reply->errorString() );
				return false;
			}

			const bool isStatusCodeValid = p_reply->isStatusCodeValid();

			if ( !isStatusCodeValid )
			{
				VTX_ERROR( "Invalid HTTP response" );
				return false;
			}

			const int status = p_reply->getStatusCode();

			if ( status != 200 )
			{
				VTX_ERROR( "HTTP " + std::to_string( status ) );
				VTX_ERROR( p_reply->getReason() );
				return false;
			}

			return true;
		}
	} // namespace Network
} // namespace VTX
