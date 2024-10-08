#ifndef __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__
#define __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__

#include <optional>
#include <string>
#include <string_view>
#include <util/callback.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Network
{
	class NetworkSystem
	{
	  public:
		/**
		 * @brief Get a file from the cache or download it.
		 * @param p_filename the filename to save the file as.
		 * @param p_data the downloaded file data.
		 * @param p_url the URL to download from if file not exists.
		 */
		void getFile(
			const std::string_view			 p_filename,
			std::string * const				 p_data,
			const std::optional<std::string> p_url = std::nullopt
		);

		/**
		 * @brief Download a file from a URL.
		 * @param p_url the URL to download from.
		 * @param p_filename the filename to save the file as in the cache.
		 * @param p_data the downloaded file data.
		 * @param p_overwrite whether to overwrite the file if it exists.
		 */
		void downloadFile(
			const std::string &	   p_url,
			const std::string_view p_filename,
			std::string * const	   p_data,
			const bool			   p_overwrite = false
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
