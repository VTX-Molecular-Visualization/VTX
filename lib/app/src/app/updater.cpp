#include "app/updater.hpp"
#include "app/events.hpp"
#include "app/infos.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/exceptions.hpp>
#include <util/string.hpp>

namespace VTX::App
{

	void Updater::checkForUpdate()
	{
		// TODO: use network manager?

		Util::Network::httpRequestGetAsync(
			UPDATER_URL,
			[ this ]( const std::string & p_text )
			{
				try
				{
					_document = Util::JSon::Document::createFromString( p_text );

					if ( not _document.json().contains( "tag_name" ) )
					{
						throw VTXException( "tag not found" );
					}

					const std::string			   tagName		= _document.json()[ "tag_name" ].getString();
					const std::vector<std::string> versionParts = Util::String::split( tagName, '.' );

					if ( versionParts.size() < 3 )
					{
						throw VTXException( "can not read version number" );
					}

					const uint major = std::stoul( versionParts[ 0 ] );
					const uint minor = std::stoul( versionParts[ 1 ] );
					const uint patch = std::stoul( versionParts[ 2 ] );

					// Check if new version is available.
					if ( major > VERSION_MAJOR || ( major == VERSION_MAJOR && minor > VERSION_MINOR )
						 || ( major == VERSION_MAJOR && minor == VERSION_MINOR && patch > VERSION_PATCH ) )
					{
						VTX_INFO( "New version found: {}", tagName );
						HUB().trigger<Events::UpdateAvailable>( major, minor, patch );
					}
					else
					{
						VTX_INFO( "Up to date: {}", tagName );
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Updater error: {}", p_e.what() );
				}
			}
		);
	}

	void Updater::downloadUpdate()
	{
		if ( not _document.json().contains( "assets" ) )
		{
			throw VTXException( "Updater can not retrieve assets" );
		}

		const Util::JSon::Array assets = _document.json()[ "assets" ].getArray();

#ifdef _WIN32
		const FilePath extension = ".exe";
#else
		const FilePath extension = ".gz";
#endif

		for ( const auto & asset : assets )
		{
			if ( not asset.contains( "name" ) || not asset.contains( "browser_download_url" ) )
			{
				continue;
			}

			const FilePath filename = asset[ "name" ].getString();

			if ( filename.extension() == extension )
			{
				const std::string_view url = asset[ "browser_download_url" ].getString();
				VTX_DEBUG( "Update url: {}", url );

				return;
			}
		}

		throw VTXException( "Updater can not retrieve file to download" );
	}
} // namespace VTX::App
