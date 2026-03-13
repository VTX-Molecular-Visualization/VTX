#include "app/updater.hpp"
#include "app/events.hpp"
#include "app/infos.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <velopack/include/Velopack.hpp>

namespace VTX::App
{
	/**
	 * @brief Application updater.
	 */
	struct Updater::Impl
	{
		/**
		 * @brief Manager, empty if dev mode.
		 */
		std::optional<Velopack::UpdateManager> manager;

		/**
		 * @brief Available update.
		 */
		std::optional<Velopack::UpdateInfo> pendingUpdate;
	};

	Updater::Updater() : _impl( std::make_unique<Impl>() )
	{
		try
		{
			_impl->manager.emplace( UPDATER_URL.data() );
		}
		catch ( const std::exception & p_e )
		{
			VTX_WARNING( "Updater not available" );
			VTX_DEBUG( "{}", p_e.what() );
		}
	}

	Updater::~Updater() = default;

	void Updater::checkForUpdate()
	{
		if ( not _impl->manager )
		{
			return;
		}

		try
		{
			auto update = ( *_impl->manager ).CheckForUpdates();
			if ( update.has_value() )
			{
				_impl->pendingUpdate = std::move( update );
				const auto & release = _impl->pendingUpdate->TargetFullRelease;
				VTX_INFO( "New version found: {}", release.Version );
				HUB().trigger<Events::UpdateAvailable>( release.Version, release.NotesMarkdown, release.NotesHtml );
			}
			else
			{
				VTX_INFO( "Up to date" );
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Updater error: {}", p_e.what() );
		}
	}

	void Updater::downloadUpdate()
	{
		assert( _impl->pendingUpdate );

		( *_impl->manager ).DownloadUpdates( *_impl->pendingUpdate );
		( *_impl->manager ).WaitExitThenApplyUpdates( *_impl->pendingUpdate );
	}
} // namespace VTX::App
