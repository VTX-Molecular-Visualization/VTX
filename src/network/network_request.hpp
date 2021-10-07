#ifndef __VTX_NETWORK_REQUEST__
#define __VTX_NETWORK_REQUEST__

#include <QNetworkReply>
#include <QNetworkRequest>

namespace VTX::Network
{
	class NetworkManager;
}

#define VTX_REQUEST friend VTX::Network::NetworkManager;

namespace VTX
{
	namespace Network
	{
		class NetworkRequest : public QNetworkRequest
		{
			VTX_REQUEST

		  public:
			NetworkRequest( const std::string & p_url ) { setUrl( QUrl( p_url.c_str() ) ); }
			virtual ~NetworkRequest() = default;

		  private:
			void		 _finished( QNetworkReply * const p_reply );
			virtual void _success( QNetworkReply * const ) = 0;
			virtual void _error( QNetworkReply * const ) {}

			bool _validateReply( QNetworkReply * const p_reply );
		};
	} // namespace Network
} // namespace VTX
#endif
