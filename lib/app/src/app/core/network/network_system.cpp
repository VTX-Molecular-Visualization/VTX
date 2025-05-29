#include "app/core/network/network_system.hpp"
#include "app/filesystem.hpp"
#include <util/logger.hpp>

namespace VTX::App::Core::Network
{

	void NetworkSystem::downloadFile(
		const std::string_view				   p_url,
		const std::string_view				   p_filename,
		const Util::Network::CallbackHttpGet & p_callback
	)
	{
		VTX_INFO( "Downloading system from: {}", p_url );

		FilePath filepath = p_filename.data();

		// Download.
		std::string text;
		Util::Network::httpRequestGet( p_url, text );

		// Save to cache.
		Util::Filesystem::writeFile( Filesystem::getCachePath( filepath ), text );
		onFileCached();
		p_callback( text );
	}

} // namespace VTX::App::Core::Network
