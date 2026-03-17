#ifndef __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__
#define __VTX_APP_CORE_NETWORK_NETWORK_SYSTEM__

#include "app/events.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <util/event_hub.hpp>
#include <util/network.hpp>
#include <vector>

namespace VTX::App::Network
{
	// TODO: rename to FileManager?
	class NetworkManager
	{
	  public:
		using DownloadId = uint64_t;

		NetworkManager();
		~NetworkManager();

		/**
		 * @brief Download a file from a URL and save it to a specified filename.
		 */
		DownloadId downloadFile(
			const std::string_view				   p_url,
			const std::string_view				   p_filename,
			const Util::Network::CallbackHttpGet & p_callback
		);

	  private:
		/**
		 * @brief Download operation struct.
		 */
		struct DownloadResult
		{
			DownloadId					   id;
			std::string					   url;
			std::string					   data;
			bool						   success;
			Util::Network::CallbackHttpGet callback;
		};

		/**
		 * @brief Download ID.
		 */
		std::atomic<DownloadId> _nextId = 0;

		/**
		 * @brief Mutex for sync.
		 */
		std::mutex _mutex;

		/**
		 * @brief Pending download.
		 */
		std::unordered_map<DownloadId, float> _pendingProgress;

		/**
		 * @brief Completed downloads.
		 */
		std::vector<DownloadResult> _completedDownloads;

		/**
		 * @brief Connection to the update event.
		 */
		Util::EventHub::Connection _updateConnection;

		/**
		 * @brief Update function to process completed downloads and update progress.
		 */
		void _onUpdate( const Events::Update & );
	};

} // namespace VTX::App::Network

#endif
