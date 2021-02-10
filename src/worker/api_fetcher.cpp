#include "api_fetcher.hpp"
#include "exception.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		ApiFetcher::ApiFetcher( const std::string & p_url ) : _url( QUrl( p_url.c_str() ) )
		{
			connect( &VTX_NETWORK_MANAGER(), &QNetworkAccessManager::finished, this, &ApiFetcher::_onReplyFinished );
		}

		void ApiFetcher::work() { VTX_NETWORK_MANAGER().get( QNetworkRequest( _url ) ); }

		void ApiFetcher::_onReplyFinished( QNetworkReply * const p_reply )
		{
			*_buffer	= p_reply->readAll().toStdString();
			_progress	= 1.f;
			_isFinished = true;
		}
	} // namespace Worker
} // namespace VTX
