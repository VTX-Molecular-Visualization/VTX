#include "app/old/internal/network/request/check_update.hpp"
#include "app/old/application/define.hpp"
// #include "ui/dialog.hpp"
#include <nlohmann/json.hpp>
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Network::Request
{
	CheckUpdate::CheckUpdate( const bool p_showPopupIfNoUpdate ) :
		NetworkRequest( std::string( Application::VTX_VERSION_URL ) ), _showPopupIfNoUpdate( p_showPopupIfNoUpdate )
	{
		VTX_INFO( "Checking for update" );
	}

	void CheckUpdate::_success( Core::Network::NetworkReply * const p_reply )
	{
		try
		{
			nlohmann::json json = nlohmann::json::parse( p_reply->readAll() ).at( "VERSION" );

			const int major	   = json.at( "MAJOR" );
			const int minor	   = json.at( "MINOR" );
			const int revision = json.at( "REVISION" );

			bool needUpdate = false;
			if ( major > Application::VTX_VERSION_MAJOR )
			{
				needUpdate = true;
			}
			else if ( major == Application::VTX_VERSION_MAJOR && minor > Application::VTX_VERSION_MINOR )
			{
				needUpdate = true;
			}
			else if ( major == Application::VTX_VERSION_MAJOR && minor == Application::VTX_VERSION_MINOR
					  && revision > Application::VTX_VERSION_REVISION )
			{
				needUpdate = true;
			}

			if ( needUpdate )
			{
				const std::string consoleMessage = "VTX " + std::to_string( major ) + "." + std::to_string( minor )
												   + "." + std::to_string( revision ) + " is available at:\n"
												   + Application::VTX_RELEASES_URL + "\nor\n"
												   + Application::VTX_WEBSITE_URL;
				VTX_INFO( "{}", consoleMessage );

				// TODO reimplement this out of VTX_APP
				// const QString dialogMessage = QString::fromStdString(
				//	"VTX " + std::to_string( major ) + "." + std::to_string( minor ) + "." + std::to_string( revision )
				//	+ " is available at: " + VTX_RELEASES_CLICKABLE_URL + "<br>or " + VTX_WEBSITE_CLICKABLE_URL );
				// UI::Dialog::openInformationDialog( "New version available", dialogMessage );
			}
			else
			{
				if ( _showPopupIfNoUpdate )
				{
					// TODO reimplement this out of VTX_APP
					// UI::Dialog::openInformationDialog( "No update", "You have the latest version" );
				}
				VTX_INFO( "No update available" );
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
		}
	}
} // namespace VTX::App::Old::Internal::Network::Request