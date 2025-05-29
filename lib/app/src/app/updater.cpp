#include "app/updater.hpp"
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
				_document = Util::JSon::Document::createFromString( p_text );

				if ( not _document.json().contains( "tag_name" ) )
				{
					throw VTXException( "Updater can not retrieve last version." );
				}

				std::string tagName = _document.json()[ "tag_name" ].getString();
				VTX_INFO( "[UPDATER] Last version found: {}", tagName );

				std::vector<std::string> versionParts = Util::String::split( tagName, '.' );

				if ( versionParts.size() < 3 )
				{
					throw VTXException( "Updater can not deduce last version." );
				}

				try
				{
					uint major = std::stoul( versionParts[ 0 ] );
					uint minor = std::stoul( versionParts[ 1 ] );
					uint patch = std::stoul( versionParts[ 2 ] );

					// Check if new version is available.
					onUpdateAvailable( major, minor, patch );
				}
				catch ( const std::exception & )
				{
					throw VTXException( "Updater can not deduce last version." );
				}
			}
		);
	}

	void Updater::downloadUpdate()
	{
		if ( not _document.json().contains( "assets" ) )
		{
			throw VTXException( "Updater can not retrieve assets." );
		}

		Util::JSon::Array assets = _document.json()[ "assets" ].getArray();
	}

} // namespace VTX::App
