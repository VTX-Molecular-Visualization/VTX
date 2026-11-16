#include "app/helper/io.hpp"
#include "app/constants.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <algorithm>
#include <cctype>
#include <util/string.hpp>

namespace VTX::App::Helper::IO
{
	bool isValidSystemId( const std::string_view p_systemId )
	{
		return not p_systemId.empty()
			   && std::ranges::all_of(
				   p_systemId, []( const unsigned char p_char ) { return std::isalnum( p_char ) != 0 || p_char == '_'; }
			   );
	}

	std::string getSystemDownloadUrl( const std::string_view p_systemId, const std::string_view p_urlTemplate )
	{
		return Util::String::replaceFirst(
			std::string( p_urlTemplate ), std::string( SYSTEM_REPLACEMENT_TOKEN ), std::string( p_systemId )
		);
	}

	FilePath getSystemDownloadFilename( const std::string_view p_systemId )
	{ return FilePath( std::string( p_systemId ) + std::string( RCSB_PDB_DOWNLOAD_FILE_EXTENSION ) ); }

	std::optional<std::string> fetch( const std::string_view p_url, const Util::Network::ProgressCallback & p_progress )
	{
		bool		cancelled = false;
		std::string data;
		Util::Network::httpRequestGet(
			p_url,
			data,
			[ &p_progress, &cancelled ]( const size_t p_downloaded, const size_t p_total ) -> bool
			{
				if ( p_progress && not p_progress( p_downloaded, p_total ) )
				{
					cancelled = true;
					return false;
				}
				return true;
			}
		);
		if ( cancelled )
		{
			return std::nullopt;
		}

		return data;
	}

	void writeCache( const std::string_view p_filename, const std::string & p_data )
	{ Util::Filesystem::writeFile( SESSION().getCacheDir() / p_filename, p_data ); }

} // namespace VTX::App::Helper::IO
