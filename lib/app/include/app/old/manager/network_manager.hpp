#ifndef __VTX_NETWORK_MANAGER__
#define __VTX_NETWORK_MANAGER__

// #define QT_NO_BEARERMANAGEMENT

#include "app/old/core/network/network_request.hpp"
#include <memory>
// #include <QNetworkAccessManager>
// #include <QObject>

namespace VTX::App::Old::Manager
{
	// TODO reimplemente that without Qt
	class NetworkManager // : public QObject
	{
		// Q_OBJECT

	  public:
		inline static NetworkManager & get()
		{
			static NetworkManager instance;
			return instance;
		}

		void sendRequest( const std::shared_ptr<Core::Network::NetworkRequest> & );

	  private:
		// std::map<QNetworkReply * const, NetworkRequest * const> _mapReplyRequest
		//	= std::map<QNetworkReply * const, NetworkRequest * const>();

		NetworkManager();
		NetworkManager( const NetworkManager & )			 = delete;
		NetworkManager & operator=( const NetworkManager & ) = delete;
		~NetworkManager()									 = default;

		// QNetworkAccessManager _networkManager = QNetworkAccessManager();

		// void _errorOccured( const QNetworkReply::NetworkError & );
		// void _sslErrors( const QList<QSslError> & );
		// void _downloadProgress( const qint64, const qint64 );
		void _finished();
	};
} // namespace VTX::App::Old::Manager
#endif
