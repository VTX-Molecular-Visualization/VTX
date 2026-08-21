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
#include <util/thread/base_thread.hpp>
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

		/**
		 * @brief Stop a download by ID.
		 */
		void stopDownload( DownloadId p_id );

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
		 * @brief Map from DownloadId to thread ID for cancellation.
		 */
		std::unordered_map<DownloadId, Util::Thread::BaseThread::ID> _activeDownload;

		/**
		 * @brief Completed downloads.
		 */
		std::vector<DownloadResult> _completedDownloads;

		/**
		 * @brief Connection for update events to dispatch completed downloads.
		 */
		void _onUpdate( const Events::Update & );
	};

} // namespace VTX::App::Network

#endif
