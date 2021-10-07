#include "network_request.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Network
	{
		void NetworkRequest::finished( QNetworkReply * const p_reply )
		{
			_validateReply( p_reply ) ? success( p_reply ) : error( p_reply );
		}

		bool NetworkRequest::_validateReply( QNetworkReply * const p_reply )
		{
			if ( p_reply->error() )
			{
				VTX_ERROR( p_reply->errorString().toStdString() );
				return false;
			}

			QVariant statusCode = p_reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
			if ( !statusCode.isValid() )
			{
				VTX_ERROR( "Invalid HTTP response" );
				return false;
			}
			int status = statusCode.toInt();

			if ( status != 200 )
			{
				VTX_ERROR( "HTTP " + std::to_string( status ) );
				QString reason = p_reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
				VTX_ERROR( reason.toStdString() );
				return false;
			}

			return true;
		}
	} // namespace Network
} // namespace VTX
