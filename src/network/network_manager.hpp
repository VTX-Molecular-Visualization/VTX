#ifndef __VTX_NETWORK_MANAGER__
#define __VTX_NETWORK_MANAGER__

#define QT_NO_BEARERMANAGEMENT

#include "network_request.hpp"
#include <QNetworkAccessManager>
#include <QObject>

namespace VTX
{
	namespace Network
	{
		class NetworkManager : public QObject
		{
			Q_OBJECT

		  public:
			inline static NetworkManager & get()
			{
				static NetworkManager instance;
				return instance;
			}

			void sendRequest( NetworkRequest * const );

		  private:
			std::map<QNetworkReply * const, NetworkRequest * const> _mapReplyRequest
				= std::map<QNetworkReply * const, NetworkRequest * const>();

			NetworkManager()						 = default;
			NetworkManager( const NetworkManager & ) = delete;
			NetworkManager & operator=( const NetworkManager & ) = delete;
			~NetworkManager()									 = default;

			QNetworkAccessManager _networkManager = QNetworkAccessManager();

			void _errorOccured( const QNetworkReply::NetworkError & );
			void _sslErrors( const QList<QSslError> & );
			void _downloadProgress( const qint64, const qint64 );
			void _finished();
		};
	} // namespace Network

	inline Network::NetworkManager & VTX_NETWORK_MANAGER() { return Network::NetworkManager::get(); }
} // namespace VTX
#endif
