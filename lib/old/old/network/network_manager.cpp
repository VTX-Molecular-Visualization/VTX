#include "network_manager.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "tool/logger.hpp"
#include <QNetworkRequest>
#include <QSslSocket>

namespace VTX
{
	namespace Network
	{
		NetworkManager::NetworkManager()
		{
			VTX_INFO( "OpenSSL version : " + QSslSocket::sslLibraryBuildVersionString().toStdString() );
		}

		void NetworkManager::sendRequest( NetworkRequest * const p_request )
		{
			QNetworkReply * const reply = _networkManager.get( *p_request );
			_mapReplyRequest.emplace( reply, p_request );

			connect( reply, &QNetworkReply::errorOccurred, this, &NetworkManager::_errorOccured );
			connect( reply, &QNetworkReply::sslErrors, this, &NetworkManager::_sslErrors );
			connect( reply, &QNetworkReply::downloadProgress, this, &NetworkManager::_downloadProgress );
			connect( reply, &QNetworkReply::finished, this, &NetworkManager::_finished );
		}

		void NetworkManager::_finished()
		{
			QNetworkReply * const  reply = qobject_cast<QNetworkReply *>( sender() );
			NetworkRequest * const req	 = _mapReplyRequest[ reply ];
			req->_finished( reply );
			delete req;
		}

		void NetworkManager::_errorOccured( const QNetworkReply::NetworkError & p_error )
		{
			QNetworkReply * const  reply = qobject_cast<QNetworkReply *>( sender() );
			NetworkRequest * const req	 = _mapReplyRequest[ reply ];
			req->_error( reply );
			VTX_DEBUG( "Network error: " + std::to_string( p_error ) );
		}

		void NetworkManager::_sslErrors( const QList<QSslError> & p_sslErrors )
		{
			QNetworkReply * const  reply = qobject_cast<QNetworkReply *>( sender() );
			NetworkRequest * const req	 = _mapReplyRequest[ reply ];
			req->_error( reply );
			VTX_ERROR( QSslSocket::sslLibraryBuildVersionString().toStdString() );
			VTX_ERROR( QSslSocket::sslLibraryVersionString().toStdString() );
			for ( int i = 0; i < p_sslErrors.size(); i++ )
			{
				VTX_ERROR( p_sslErrors.at( i ).errorString().toStdString() );
			}
		}

		void NetworkManager::_downloadProgress( const qint64 p_bytesReceived, const qint64 p_bytesTotal )
		{
			if ( p_bytesReceived == 0 )
			{
				return;
			}

			float percent = p_bytesReceived / p_bytesTotal;
			// VTX_DEBUG( std::to_string( p_bytesReceived ) + " / " + std::to_string( p_bytesTotal ) );
			VTX_EVENT( new Event::VTXEventValue<float>( Event::Global::UPDATE_PROGRESS_BAR, percent ) );
			// VTX_INFO( std::to_string( ( uint )( percent * 100 ) ) + "%" );
		}

	} // namespace Network
} // namespace VTX
