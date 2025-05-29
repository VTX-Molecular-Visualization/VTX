#include "app/updater.hpp"
#include <util/json/json.hpp>
#include <util/string.hpp>

namespace VTX::App
{

	void Updater::checkForUpdate()
	{
		// TODO: use network manager?
		const std::string_view url = "https://api.github.com/repos/VTX-Molecular-Visualization/VTX/releases/latest";

		Util::Network::httpRequestGetAsync(
			url,
			[]( const std::string & p_text )
			{
				Util::JSon::Document doc = Util::JSon::Document::createFromString( p_text );

				if ( not doc.json().contains( "tag_name" ) )
				{
					VTX_ERROR( "[UPDATER] Tag name not found" );
					return;
				}

				std::string tagName = doc.json()[ "tag_name" ].getString();
				VTX_INFO( "[UPDATER] Last version found: {}", tagName );

				std::vector<std::string> versionParts = Util::String::split( tagName, '.' );

				if ( versionParts.size() < 3 )
				{
					VTX_ERROR( "[UPDATER] Invalid version format: {}", tagName );
					return;
				}

				try
				{
					uint major = std::stoul( versionParts[ 0 ] );
					uint minor = std::stoul( versionParts[ 1 ] );
					uint patch = std::stoul( versionParts[ 2 ] );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "[UPDATER] Error parsing version numbers: {}", p_e.what() );
					return;
				}
			}
		);
	}

} // namespace VTX::App
