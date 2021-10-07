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
			connect( reply, &QNetworkReply::finished, this, &NetworkManager::_finishedDownloadMMTF );
		}

		void NetworkManager::checkUpdate()
		{
			VTX_INFO( "Checking for update" );
			QNetworkRequest request;
			request.setUrl( QUrl( std::string( VTX_VERSION_URL ).c_str() ) );
			QNetworkReply * const reply = _networkManager.get( request );
			connect( reply, &QNetworkReply::errorOccurred, this, &NetworkManager::_errorOccured );
			connect( reply, &QNetworkReply::sslErrors, this, &NetworkManager::_sslErrors );
			connect( reply, &QNetworkReply::finished, this, &NetworkManager::_finishedCheckUpdate );
		}

		void NetworkManager::_finishedDownloadMMTF()
		{
			QNetworkReply * const replyThis = qobject_cast<QNetworkReply *>( sender() );
			if ( _isValidReply( replyThis ) == false )
			{
				return;
			}

			VTX_INFO( "Downloaded" );

			std::map<IO::FilePath, std::string *> mapBuffers = std::map<IO::FilePath, std::string *>();
			mapBuffers.emplace( IO::FilePath( _id + ".mmtf" ), new std::string( replyThis->readAll().toStdString() ) );

			VTX_ACTION( new Action::Main::Open( mapBuffers ) );
		}

		void NetworkManager::_finishedCheckUpdate()
		{
			QNetworkReply * const replyThis = qobject_cast<QNetworkReply *>( sender() );
			if ( _isValidReply( replyThis ) == false )
			{
				return;
			}

			try
			{
				nlohmann::json json = nlohmann::json::parse( replyThis->readAll().toStdString() ).at( "VERSION" );

				int major	 = json.at( "MAJOR" );
				int minor	 = json.at( "MINOR" );
				int revision = json.at( "REVISION" );

				if ( major != VTX_VERSION_MAJOR || minor != VTX_VERSION_MINOR || revision != VTX_VERSION_REVISION )
				{
					std::string message = "VTX " + std::to_string( major ) + "." + std::to_string( minor ) + "."
										  + std::to_string( revision ) + " is available at:\n" + VTX_RELEASES_URL
										  + "\nor\n" + VTX_WEBSITE_URL;

					VTX_INFO( message );
					UI::Dialog::openInformationDialog( "New version available", QString::fromStdString( message ) );
				}
				else
				{
					VTX_INFO( "No update available" );
				}
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( p_e.what() );
			}
		}

		bool NetworkManager::_isValidReply( const QNetworkReply * const p_reply )
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
