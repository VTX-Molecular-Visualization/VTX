#ifndef __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__
#define __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__

#include "app/filesystem.hpp"
#include <string_view>
#include <util/logger.hpp>
#include <util/network.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Network
{

	class NetworkSystem
	{
	  public:
		/**
		 * @brief Download a file from a URL and save it to cache.
		 * @param p_filename the filename to save the file as.
		 * @param p_data the downloaded file data.
		 * @param p_url the URL to download from.
		 */
		void getFile(
			const std::string_view			 p_filename,
			std::string * const				 p_data,
			const std::optional<std::string> p_url = std::nullopt
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

		void downloadFile(
			const std::string &	   p_url,
			const std::string_view p_filename,
			std::string * const	   p_data,
			const bool			   p_overwrite = false
		)
		{
			// assert( not std::filesystem::exists( cachePath ) );
			if ( not p_overwrite and std::filesystem::exists( Filesystem::getCachePath( p_filename ) ) )
			{
				assert( 1 );
			}

			VTX_INFO( "Downloading molecule from: {}", p_url );
			// Download.
			Util::Network::httpRequestGet( p_url, p_data );
			// Save to cache.
			Util::Filesystem::writeFile( Filesystem::getCachePath( p_filename ), *p_data );
		}

	  private:
	};

} // namespace VTX::App::Core::Network

namespace VTX::App
{
	inline Core::Network::NetworkSystem & NETWORK_SYSTEM()
	{
		return Util::Singleton<Core::Network::NetworkSystem>::get();
	}
} // namespace VTX::App

#endif
