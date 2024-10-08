#include "app/core/network/network_system.hpp"
#include "app/filesystem.hpp"
#include <util/logger.hpp>
#include <util/network.hpp>

namespace VTX::App::Core::Network
{

	void NetworkSystem::getFile(
		const std::string_view			 p_filename,
		std::string * const				 p_data,
		const std::optional<std::string> p_url
	)
	{
		const FilePath cachePath = Filesystem::getCachePath( p_filename );
		if ( std::filesystem::exists( cachePath ) )
		{
			VTX_INFO( "Loading molecule from cache: {}", cachePath.string() );
			*p_data = Util::Filesystem::readPath( cachePath );
		}
		else
		{
			if ( p_url.has_value() )
			{
				VTX_INFO( "Downloading molecule from: {}", p_url.value() );
				downloadFile( p_url.value(), p_filename, p_data );
			}
			else
			{
				throw HTTPException( "File not found in cache and no URL provided." );
			}
		}
	}

	void NetworkSystem::downloadFile(
		const std::string &	   p_url,
		const std::string_view p_filename,
		std::string * const	   p_data,
		const bool			   p_overwrite
	)
	{
		// TODO: assert or exception?
		if ( not p_overwrite and std::filesystem::exists( Filesystem::getCachePath( p_filename ) ) )
		{
			assert( 1 );
		}

		VTX_INFO( "Downloading molecule from: {}", p_url );
		// Download.
		Util::Network::httpRequestGet( p_url, p_data );
		// Save to cache.
		Util::Filesystem::writeFile( Filesystem::getCachePath( p_filename ), *p_data );
		onFileCached();
	}

} // namespace VTX::App::Core::Network
