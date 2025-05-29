#ifndef __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__
#define __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__

#include <optional>
#include <string>
#include <string_view>
#include <util/callback.hpp>
#include <util/network.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Network
{
	class NetworkSystem
	{
	  public:
		/**
		 * @brief Download a file from a URL.
		 * @param p_url the URL to download from.
		 * @param p_filename the filename to save the file as in the cache.
		 * @param p_callback the callback to call when downloaded.
		 */
		void downloadFile(
			const std::string_view				   p_url,
			const std::string_view				   p_filename,
			const Util::Network::CallbackHttpGet & p_callback
		);

		Util::Callback<> onFileCached;
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
