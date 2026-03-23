#include "app/session.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/application.hpp"
#include "app/args.hpp"
#include "app/constants.hpp"
#include "app/services.hpp"
#include "app/threading/thread_manager.hpp"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <velopack/include/Velopack.hpp>

#if defined( __linux__ )
#include <unistd.h>
#endif

using namespace VTX::Util;

namespace VTX::App
{

	/**
	 * @brief Data.
	 */
	struct Session::Impl
	{
		/**
		 * @brief Manager, empty if dev mode.
		 */
		std::optional<Velopack::UpdateManager> manager;

		/**
		 * @brief Available update.
		 */
		std::optional<Velopack::UpdateInfo> pendingUpdate;

		/**
		 * @brief Set by the worker thread when CheckForUpdates completes.
		 */
		std::atomic<bool> updateCheckReady = false;

		/**
		 * @brief True while an update check is active and until its result is consumed on the main thread.

		 */
		std::atomic<bool> updateCheckInProgress = false;

		/**
		 * @brief True while an update download/apply is active.
		 */
		std::atomic<bool> updateDownloadInProgress = false;

		/**
		 * @brief Held while waiting for the update check result on the main thread.
		 */
		Util::EventHub::Connection updateCheckConnection;
	};

	namespace
	{
		enum class SessionPathRoot
		{
			Data,
			Pictures
		};

		FilePath _getPortableBaseDir()
		{
#if defined( __linux__ )
			if ( const char * appImagePath = std::getenv( "APPIMAGE" );
				 appImagePath != nullptr && appImagePath[ 0 ] != '\0' )
			{
				return FilePath( appImagePath ).parent_path();
			}
#endif
			return Filesystem::getExecutableDir();
		}

		bool _isWritableDirectory( const FilePath & p_path )
		{
#if defined( __linux__ )
			std::error_code ec;
			FilePath		candidate = p_path;
			while ( not candidate.empty() && not std::filesystem::exists( candidate, ec ) )
			{
				candidate = candidate.parent_path();
			}

			if ( candidate.empty() || ec )
			{
				return false;
			}

			return access( candidate.string().c_str(), W_OK ) == 0;
#else
			(void)p_path;
			return true;
#endif
		}

		FilePath _getDefaultBaseDir( const SessionPathRoot p_root )
		{
			switch ( p_root )
			{
			case SessionPathRoot::Data: return Filesystem::getDataHome();
			case SessionPathRoot::Pictures: return Filesystem::getPicturesFolder();
			}

			return Filesystem::getDataHome();
		}

		FilePath _resolveAppDir( const bool p_isPortable, const SessionPathRoot p_root )
		{
			if ( p_isPortable )
			{
				const FilePath portableDir = _getPortableBaseDir();
				if ( _isWritableDirectory( portableDir ) )
				{
					return portableDir;
				}
			}

			return _getDefaultBaseDir( p_root ) / APP_FOLDER_NAME;
		}
	} // namespace

	Session::Session() : _impl( std::make_unique<Impl>() )
	{
		try
		{
			Velopack::VelopackApp::Build()
				.SetAutoApplyOnStartup( false )
				//.OnAfterInstall(  )
				.OnBeforeUninstall( []( void *, const char * )
									{ std::filesystem::remove_all( Filesystem::getDataHome() / APP_FOLDER_NAME ); } )
				//.OnBeforeUpdate(  )
				//.OnAfterUpdate(  )
				//.OnFirstRun(  )
				//.OnRestarted(  )
				.Run();

			auto src = std::make_unique<Velopack::GithubSource>( URL_UPDATE.data() );
			_impl->manager.emplace( std::move( src ) );
			//_impl->manager.emplace( URL_UPDATE.data() );
		}
		catch ( const std::exception & p_e )
		{
			VTX_DEBUG( "{}", p_e.what() );
		}
	}

	Session::~Session() = default;

	void Session::checkForUpdate()
	{
		if ( not _impl->manager )
		{
			return;
		}

		if ( _impl->updateCheckInProgress.exchange( true ) )
		{
			VTX_INFO( "Update check already in progress" );
			return;
		}

		_impl->updateCheckReady = false;
		_impl->pendingUpdate.reset();

		THREAD().createThread(
			[ this ]( App::Threading::BaseThread & p_thread ) -> uint
			{
				p_thread.setProgressText( "Checking for updates..." );
				try
				{
					auto update = ( *_impl->manager ).CheckForUpdates();
					if ( update.has_value() )
					{
						_impl->pendingUpdate = std::move( update );
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Updater error: {}", p_e.what() );
				}
				_impl->updateCheckReady = true;
				return 0;
			}
		);

		_impl->updateCheckConnection = HUB().connect<Events::Update, &Session::_onUpdateCheckResult>( this );
	}

	void Session::_onUpdateCheckResult( const Events::Update & )
	{
		if ( not _impl->updateCheckReady.exchange( false ) )
		{
			return;
		}

		_impl->updateCheckInProgress = false;
		HUB().disconnect( _impl->updateCheckConnection );

		if ( _impl->pendingUpdate )
		{
			const auto & release = _impl->pendingUpdate->TargetFullRelease;
			VTX_INFO( "New version found: {}", release.Version );
			VTX_DEBUG( "Release notes MD:\n{}", release.NotesMarkdown );
			VTX_DEBUG( "Release notes HTML:\n{}", release.NotesHtml );
			HUB().trigger<Events::UpdateAvailable>( version(), release.Version, release.NotesHtml, release.Size );
		}
		else
		{
			VTX_INFO( "Up to date" );
		}
	}

	void Session::downloadUpdate()
	{
		if ( not _impl->manager )
		{
			VTX_WARNING( "downloadUpdate called without update manager" );
			return;
		}

		if ( _impl->updateCheckInProgress )
		{
			VTX_INFO( "downloadUpdate ignored while update check is still in progress" );
			return;
		}

		if ( not _impl->pendingUpdate )
		{
			VTX_WARNING( "downloadUpdate called without a pending update" );
			return;
		}

		if ( _impl->updateDownloadInProgress.exchange( true ) )
		{
			VTX_INFO( "Update download already in progress" );
			return;
		}

		try
		{
			const Velopack::UpdateInfo pendingUpdate = *_impl->pendingUpdate;
			const auto &			   release	   = pendingUpdate.TargetFullRelease;

			VTX_INFO( "downloadUpdate: starting update to {}", release.Version );
			VTX_INFO( "downloadUpdate: calling DownloadUpdates" );
			( *_impl->manager ).DownloadUpdates( pendingUpdate );
			VTX_INFO( "downloadUpdate: DownloadUpdates completed" );

			const bool restart = not isPortable();
			VTX_INFO( "downloadUpdate: calling WaitExitThenApplyUpdates (restart={})", restart );
			( *_impl->manager ).WaitExitThenApplyUpdates( pendingUpdate, false, restart /*, ARGS().toStringVec()*/ );
			VTX_INFO( "downloadUpdate: WaitExitThenApplyUpdates returned" );

			VTX_INFO( "downloadUpdate: update flow completed, quitting application" );
			ACTION().execute<Action::Application::Quit>();
		}
		catch ( const std::exception & p_e )
		{
			_impl->updateDownloadInProgress = false;
			VTX_ERROR( "Update download error: {}", p_e.what() );
		}
	}

	bool Session::isPortable() const
	{
		if ( not _impl->manager )
		{
			return true;
		}
		return ( *_impl->manager ).IsPortable();
	}

	FilePath Session::getDataHome() const { return _resolveAppDir( isPortable(), SessionPathRoot::Data ); }

	FilePath Session::getPicturesFolder() const { return _resolveAppDir( isPortable(), SessionPathRoot::Pictures ); }

	FilePath Session::getShadersDir() const { return Filesystem::getExecutableDir() / "shaders"; }
	FilePath Session::getLicenseFile() const { return Filesystem::getExecutableDir() / "license.txt"; }
	FilePath Session::getReadmeFile() const { return Filesystem::getExecutableDir() / "README.md"; }
	FilePath Session::getChangelogFile() const { return Filesystem::getExecutableDir() / "CHANGELOG.md"; }
	FilePath Session::getDataDir() const { return Filesystem::getExecutableDir() / "data"; }
	FilePath Session::getResidueDataDir() const { return getDataDir() / "residue"; }
	FilePath Session::getResidueDataFilePath( const std::string_view p_residue )
	{
		return getResidueDataDir() / p_residue.substr( 0, 1 );
	}
	FilePath Session::getLogsDir() const { return getDataHome() / "logs"; }
	FilePath Session::getCacheDir() const { return getDataHome() / "cache"; }
	FilePath Session::getSnapshotsDir() const { return getPicturesFolder() / "snapshots"; }
	FilePath Session::getRepresentationsDir() const { return getDataHome() / "representations"; }
	FilePath Session::getColorLayoutsDir() const { return getDataHome() / "colors"; }
	FilePath Session::getEffectsDir() const { return getDataHome() / "effects"; }
	FilePath Session::getConfigIniFile() const { return getDataHome() / "config.ini"; }
	// const FilePath Session::getSettingJsonFile() const { return USER_DATA_DIR / "setting.json"; }

	void Session::print() const
	{
		VTX_DEBUG( "Version from Conan: {}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH );
		if ( _impl->manager )
		{
			VTX_DEBUG( "Version from Velopack: {}", ( *_impl->manager ).GetCurrentVersion() );
			VTX_DEBUG( "AppId: {}", ( *_impl->manager ).GetAppId() );
			VTX_DEBUG( "Is portable: {}", ( *_impl->manager ).IsPortable() );
		}
		else
		{
			VTX_DEBUG( "No app manager available" );
		}
		VTX_DEBUG( "Executable dir: {}", Filesystem::getExecutableDir().string() );
		VTX_DEBUG( "Data home: {}", getDataHome().string() );
		VTX_DEBUG( "Pictures folder: {}", getPicturesFolder().string() );
	}

} // namespace VTX::App
