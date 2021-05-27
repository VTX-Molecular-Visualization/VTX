#include "network_manager.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "tool/logger.hpp"
#include <QNetworkRequest>

namespace VTX
{
	namespace Network
	{
		void NetworkManager::downloadMMTF( const std::string & p_id )
		{
			_id = p_id;
			VTX_INFO( "Downloading " + p_id + "..." );

			QNetworkRequest request;
			request.setUrl( QUrl( std::string( API_URL_MMTF + p_id ).c_str() ) );
			QNetworkReply * const reply = _networkManager.get( request );
			connect( reply, &QNetworkReply::errorOccurred, this, &NetworkManager::_errorOccured );
			connect( reply, &QNetworkReply::sslErrors, this, &NetworkManager::_sslErrors );
			connect( reply, &QNetworkReply::downloadProgress, this, &NetworkManager::_downloadProgress );
			connect( reply, &QNetworkReply::finished, this, &NetworkManager::_finished );
		}

		void NetworkManager::_finished()
		{
			QNetworkReply * replyThis = qobject_cast<QNetworkReply *>( sender() );

			if ( replyThis->error() )
			{
				VTX_ERROR( replyThis->errorString().toStdString() );
				return;
			}

			QVariant statusCode = replyThis->attribute( QNetworkRequest::HttpStatusCodeAttribute );
			if ( !statusCode.isValid() )
			{
				VTX_ERROR( "Invalid HTTP response" );
				return;
			}
			int status = statusCode.toInt();

			if ( status != 200 )
			{
				VTX_ERROR( "HTTP " + std::to_string( status ) );
				QString reason = replyThis->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
				VTX_ERROR( reason.toStdString() );
				return;
			}

			VTX_INFO( "Downloaded" );

			std::map<FilePath, std::string *> mapBuffers = std::map<FilePath, std::string *>();
			mapBuffers.emplace( FilePath( _id + ".mmtf" ), new std::string( replyThis->readAll().toStdString() ) );

			VTX_ACTION( new Action::Main::Open( mapBuffers ) );
		}

		void NetworkManager::_errorOccured( const QNetworkReply::NetworkError & p_error )
		{
			VTX_DEBUG( "Network error: " + std::to_string( p_error ) );
		}

		void NetworkManager::_sslErrors( const QList<QSslError> & p_sslErrors )
		{
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
