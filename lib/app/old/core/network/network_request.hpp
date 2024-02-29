#ifndef __VTX_APP_OLD_CORE_NETWORK_NETWORK_REQUEST__
#define __VTX_APP_OLD_CORE_NETWORK_NETWORK_REQUEST__

// #include <QNetworkReply>
// #include <QNetworkRequest>
#include <string>

#define VTX_REQUEST friend VTX::App::Old::Manager::NetworkManager;

// TODO reimplements this without Qt
namespace VTX::App::Old
{
	namespace Manager
	{
		class NetworkManager;
	}

	namespace Core::Network
	{
		class NetworkReply // : public QNetworkReply
		{
		  public:
			bool error()
			{
				return true;
				// return QNetworkReply::error();
			}
			std::string errorString()
			{
				return "";
				// return QNetworkReply::errorString().toStdString();
			}

			bool isStatusCodeValid() const
			{
				return false;

				// QVariant statusCode = attribute( QNetworkRequest::HttpStatusCodeAttribute );
				// return statusCode.isValid();
			}

			int getStatusCode() const
			{
				return 0;

				// QVariant statusCode = attribute( QNetworkRequest::HttpStatusCodeAttribute );
				// return statusCode.toInt();
			}
			std::string getReason() const
			{
				return "";
				// return attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString().toStdString();
			}
			std::string readAll() const
			{
				return "";
				// return QNetworkReply::readAll().toStdString();
			}
		};

		class NetworkRequest // : public QNetworkRequest
		{
			VTX_REQUEST

		  public:
			NetworkRequest( const std::string & p_url ) { setUrl( p_url ); }
			virtual ~NetworkRequest() = default;

			void setUrl( const std::string & p_url )
			{
				// QNetworkRequest::setUrl( QUrl( p_url.c_str() ) );
			}

		  private:
			void		 _finished( NetworkReply * const p_reply );
			virtual void _success( NetworkReply * const ) = 0;
			virtual void _error( NetworkReply * const ) {}

			bool _validateReply( NetworkReply * const p_reply );
		};
	} // namespace Core::Network
} // namespace VTX::App::Old
#endif