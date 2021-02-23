#ifndef __VTX_NETWORK_MANAGER__
#define __VTX_NETWORK_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#define QT_NO_BEARERMANAGEMENT
#include <QNetworkAccessManager>
#include <QNetworkReply>
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

			void downloadMMTF( const std::string & p_id );

		  private:
			QNetworkAccessManager _networkManager = QNetworkAccessManager();

			NetworkManager()						 = default;
			NetworkManager( const NetworkManager & ) = delete;
			NetworkManager & operator=( const NetworkManager & ) = delete;
			~NetworkManager()									 = default;

			std::string _id;

			void _get( std::string & );

			void _finished();
			void _errorOccured( const QNetworkReply::NetworkError & );
			void _downloadProgress( const qint64, const qint64 );
		};
	} // namespace Network

	inline Network::NetworkManager & VTX_NETWORK_MANAGER() { return Network::NetworkManager::get(); }
} // namespace VTX
#endif
