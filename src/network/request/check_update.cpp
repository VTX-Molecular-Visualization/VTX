#include "check_update.hpp"
#include "define.hpp"
#include "ui/dialog.hpp"
#include <nlohmann/json.hpp>

namespace VTX::Network::Request
{
	CheckUpdate::CheckUpdate( const bool p_showPopupIfNoUpdate ) :
		NetworkRequest( std::string( VTX_VERSION_URL ) ), _showPopupIfNoUpdate( p_showPopupIfNoUpdate )
	{
		VTX_INFO( "Checking for update" );
	}

	void CheckUpdate::_success( QNetworkReply * const p_reply )
	{
		try
		{
			nlohmann::json json = nlohmann::json::parse( p_reply->readAll().toStdString() ).at( "VERSION" );

			const int major	   = json.at( "MAJOR" );
			const int minor	   = json.at( "MINOR" );
			const int revision = json.at( "REVISION" );

			if ( major != VTX_VERSION_MAJOR || minor != VTX_VERSION_MINOR || revision != VTX_VERSION_REVISION )
			{
				const std::string consoleMessage = "VTX " + std::to_string( major ) + "." + std::to_string( minor )
												   + "." + std::to_string( revision ) + " is available at:\n"
												   + VTX_RELEASES_URL + "\nor\n" + VTX_WEBSITE_URL;

				const QString dialogMessage = QString::fromStdString(
					"VTX " + std::to_string( major ) + "." + std::to_string( minor ) + "." + std::to_string( revision )
					+ " is available at: " + VTX_RELEASES_CLICKABLE_URL + "<br>or " + VTX_WEBSITE_CLICKABLE_URL );

				VTX_INFO( consoleMessage );
				UI::Dialog::openInformationDialog( "New version available", dialogMessage );
			}
			else
			{
				if ( _showPopupIfNoUpdate )
				{
					UI::Dialog::openInformationDialog( "No update", "You have the latest version" );
				}
				VTX_INFO( "No update available" );
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
		}
	}
} // namespace VTX::Network::Request
