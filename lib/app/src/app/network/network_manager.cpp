#include "app/network/network_manager.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include "app/threading/thread_manager.hpp"
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App::Network
{
	NetworkManager::NetworkManager()
	{
		_updateConnection = HUB().connect<Events::Update, &NetworkManager::_onUpdate>( this );
	}

	NetworkManager::~NetworkManager() { HUB().disconnect( _updateConnection ); }

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

		THREAD().createThread(
			[ this, id, url, filename, p_callback ]( App::Threading::BaseThread & ) -> uint
			{
				std::string data;
				bool		success = true;
				try
				{
					Util::Network::httpRequestGet(
						url,
						data,
						[ this, id ]( size_t p_downloaded, size_t p_total )
						{
							if ( p_total > 0 )
							{
								std::lock_guard lock( _mutex );
								_pendingProgress[ id ]
									= static_cast<float>( p_downloaded ) / static_cast<float>( p_total );
							}
						}
					);

					Util::Filesystem::writeFile( SESSION().getCacheDir() / filename, data );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Download error ({}): {}", url, p_e.what() );
					success = false;
				}

				{
					std::lock_guard lock( _mutex );
					_pendingProgress.erase( id );
					_completedDownloads.push_back( { id, url, std::move( data ), success, p_callback } );
				}

				return success ? 1u : 0u;
			}
		);

		return id;
	}

	void NetworkManager::_onUpdate( const Events::Update & )
	{
		std::unordered_map<DownloadId, float> progress;
		std::vector<DownloadResult>			  completed;
		{
			std::lock_guard lock( _mutex );
			if ( _pendingProgress.empty() && _completedDownloads.empty() )
			{
				return;
			}
			std::swap( progress, _pendingProgress );
			std::swap( completed, _completedDownloads );
		}

		for ( const auto & [ id, value ] : progress )
		{
			HUB().trigger<Events::FileDownloadProgress>( id, value );
		}

		for ( auto & result : completed )
		{
			if ( result.success )
			{
				result.callback( std::move( result.data ) );
			}
			HUB().trigger<Events::FileDownloaded>( result.id, result.success );
		}
	}

} // namespace VTX::App::Network
