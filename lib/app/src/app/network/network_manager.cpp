#include "app/network/network_manager.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Network
{
	NetworkManager::NetworkManager() { HUB().connect<Events::Update, &NetworkManager::_onUpdate>( this ); }

	NetworkManager::~NetworkManager() {}

	NetworkManager::DownloadId NetworkManager::downloadFile(
		const std::string_view				   p_url,
		const std::string_view				   p_filename,
		const Util::Network::CallbackHttpGet & p_callback
	)
	{
		VTX_INFO( "Downloading from: {}", p_url );

		const DownloadId  id	   = _nextId++;
		const std::string url	   = std::string( p_url );
		const std::string filename = std::string( p_filename );

		Util::Thread::BaseThread & thread = THREAD().createThread(
			[ this, id, url, filename, p_callback ](
				Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread
			) -> uint
			{
				std::string data;
				bool		success = true;
				p_thread.setProgressText( "Downloading " + filename );
				try
				{
					Util::Network::httpRequestGet(
						url,
						data,
						[ &p_stopToken, &p_thread ]( size_t p_downloaded, size_t p_total ) -> bool
						{
							if ( p_total > 0 )
							{
								p_thread.setProgress(
									static_cast<float>( p_downloaded ) / static_cast<float>( p_total )
								);
							}
							return not p_stopToken.stop_requested();
						}
					);

					if ( not p_stopToken.stop_requested() )
					{
						Util::Filesystem::writeFile( SESSION().getCacheDir() / filename, data );
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Download error ({}): {}", url, p_e.what() );
					success = false;
				}

				{
					std::lock_guard lock( _mutex );
					_activeDownload.erase( id );
					if ( not p_stopToken.stop_requested() )
					{
						_completedDownloads.push_back( { id, url, std::move( data ), success, p_callback } );
					}
				}

				return success ? 1u : 0u;
			}
		);

		{
			std::lock_guard lock( _mutex );
			_activeDownload[ id ] = thread.getId();
		}

		return id;
	}

	void NetworkManager::stopDownload( const DownloadId p_id )
	{
		Util::Thread::BaseThread::ID threadId;
		{
			std::lock_guard lock( _mutex );
			const auto		it = _activeDownload.find( p_id );
			if ( it == _activeDownload.end() )
			{
				return;
			}
			threadId = it->second;
		}

		Util::Thread::BaseThread * thread = THREAD().get( threadId );
		if ( thread != nullptr )
		{
			thread->stop();
		}
	}

	void NetworkManager::_onUpdate( const Events::Update & )
	{
		std::vector<DownloadResult> completed;
		{
			std::lock_guard lock( _mutex );
			if ( _completedDownloads.empty() )
			{
				return;
			}
			std::swap( completed, _completedDownloads );
		}

		for ( auto & result : completed )
		{
			if ( result.success )
			{
				result.callback( std::move( result.data ) );
			}
		}
	}

} // namespace VTX::App::Network
