#include "app/session.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/application.hpp"
#include "app/arguments.hpp"
#include "app/constants.hpp"
#include "app/services.hpp"
#include <atomic>
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/thread/thread_manager.hpp>
#include <velopack/include/Velopack.hpp>

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
		 * @brief Set by the worker thread when CheckForUpdates fails.
		 */
		std::atomic<bool> updateCheckFailed = false;

		/**
		 * @brief True while an update download/apply is active.
		 */
		std::atomic<bool> updateDownloadInProgress = false;

		/**
		 * @brief Set by the worker thread when DownloadUpdates completes.
		 */
		std::atomic<bool> updateDownloadReady = false;

		/**
		 * @brief Set by the worker thread when DownloadUpdates succeeds.
		 */
		std::atomic<bool> updateDownloadSucceeded = false;

		/**
		 * @brief True once the update payload has been downloaded and is waiting for user restart.

		 */
		std::atomic<bool> updateReadyToRestart = false;

		/**
		 * @brief Download worker thread id.
		 */
		Util::Thread::BaseThread::ID updateDownloadThreadId {};

		/**
		 * @brief Last error message produced during the download phase.
		 */
		std::string updateDownloadError;

		/**
		 * @brief Held while waiting for the update check result on the main thread.
		 */
		Util::EventHub::Connection updateCheckConnection;

		/**
		 * @brief Held while waiting for the update download result on the main thread.
		 */
		Util::EventHub::Connection updateDownloadConnection;

		/**
		 * @brief Held while waiting for the update download progress on the main thread.
		 */
		Util::EventHub::Connection updateDownloadProgressConnection;
	};

	void Session::handleStartupActivation()
	{
		try
		{
			Velopack::VelopackApp::Build()
				.SetAutoApplyOnStartup( false )
				//.OnAfterInstall(  )
				//.OnBeforeUpdate(  )
				//.OnAfterUpdate(  )
				//.OnFirstRun(  )
				//.OnRestarted(  )
				.OnBeforeUninstall( []( void *, const char * )
									{ std::filesystem::remove_all( Filesystem::getDataHome() / APP_FOLDER_NAME ); } )
				.Run();
		}
		catch ( const std::exception & p_e )
		{
			// Logger not initialized.
			std::cout << "Velopack startup hook: " << p_e.what() << std::endl;
		}
	}

	Session::Session() : _impl( std::make_unique<Impl>() )
	{
		try
		{
			auto src = std::make_unique<Velopack::GithubSource>( URL_UPDATE.data(), "", true );
			_impl->manager.emplace( std::move( src ) );
		}
		catch ( const std::exception & p_e )
		{
			// Logger not initialized yet.
			std::cerr << "Velopack update manager error: " << p_e.what() << std::endl;
		}
	}

	Session::~Session() = default;

	void Session::checkForUpdate()
	{
		if ( not _impl->manager )
		{
			VTX_INFO( "No update manager" );
			return;
		}

		if ( _impl->updateCheckInProgress.exchange( true ) )
		{
			VTX_TRACE( "Update check already in progress" );
			return;
		}

		_impl->updateCheckReady	 = false;
		_impl->updateCheckFailed = false;
		_impl->pendingUpdate.reset();

		THREAD().createThread(
			[ this ]( Util::Thread::StopToken, Util::Thread::BaseThread & p_thread ) -> uint
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
					_impl->updateCheckFailed = true;
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

		if ( _impl->updateCheckFailed )
		{
			VTX_TRACE( "Update check failed" );
			return;
		}

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
			VTX_TRACE( "downloadUpdate called without update manager" );
			return;
		}

		if ( _impl->updateCheckInProgress )
		{
			VTX_TRACE( "downloadUpdate ignored while update check is still in progress" );
			return;
		}

		if ( not _impl->pendingUpdate )
		{
			VTX_TRACE( "downloadUpdate called without a pending update" );
			return;
		}

		if ( _impl->updateDownloadInProgress.exchange( true ) )
		{
			VTX_TRACE( "Update download already in progress" );
			return;
		}

		try
		{
			const Velopack::UpdateInfo pendingUpdate = *_impl->pendingUpdate;
			const auto &			   release		 = pendingUpdate.TargetFullRelease;
			_impl->updateDownloadReady				 = false;
			_impl->updateDownloadSucceeded			 = false;
			_impl->updateReadyToRestart				 = false;
			_impl->updateDownloadError.clear();

			VTX_TRACE( "downloadUpdate: starting update to {}", release.Version );
			Util::Thread::BaseThread & downloadThread = THREAD().createThread(
				[ this, pendingUpdate ]( Util::Thread::StopToken, Util::Thread::BaseThread & p_thread ) -> uint
				{
					p_thread.setProgressText( "Downloading update..." );
					try
					{
						VTX_TRACE( "downloadUpdate: calling DownloadUpdates" );
						( *_impl->manager )
							.DownloadUpdates(
								pendingUpdate,
								[]( void * p_userData, size_t p_progress )
								{
									Util::Thread::BaseThread * thread
										= reinterpret_cast<Util::Thread::BaseThread *>( p_userData );
									thread->setProgress( float( p_progress ) / 100.f );
								},
								&p_thread
							);
						p_thread.setProgress( 1.f );
						VTX_TRACE( "downloadUpdate: DownloadUpdates completed" );
						_impl->updateDownloadSucceeded = true;
					}
					catch ( const std::exception & p_e )
					{
						_impl->updateDownloadSucceeded = false;
						_impl->updateDownloadError	   = p_e.what();
						VTX_ERROR( "Update download error: {}", p_e.what() );
					}

					_impl->updateDownloadReady = true;
					return 0;
				}
			);
			_impl->updateDownloadThreadId = downloadThread.getId();

			_impl->updateDownloadConnection = HUB().connect<Events::Update, &Session::_onUpdateDownloadResult>( this );
			_impl->updateDownloadProgressConnection
				= HUB().connect<Events::Update, &Session::_onUpdateDownloadProgress>( this );
		}
		catch ( const std::exception & p_e )
		{
			_impl->updateDownloadInProgress = false;
			VTX_ERROR( "Update download error: {}", p_e.what() );
		}
	}

	void Session::_onUpdateDownloadResult( const Events::Update & )
	{
		if ( not _impl->updateDownloadReady.exchange( false ) )
		{
			return;
		}

		HUB().disconnect( _impl->updateDownloadConnection );
		HUB().disconnect( _impl->updateDownloadProgressConnection );

		if ( not _impl->updateDownloadSucceeded )
		{
			_impl->updateDownloadInProgress = false;
			VTX_TRACE( "downloadUpdate: download phase failed" );
			HUB().trigger<Events::UpdateDownloadFailed>(
				_impl->updateDownloadError.empty() ? "Update download failed." : _impl->updateDownloadError
			);
			return;
		}

		if ( not _impl->manager || not _impl->pendingUpdate )
		{
			_impl->updateDownloadInProgress = false;
			VTX_TRACE( "downloadUpdate: apply phase aborted due to invalid updater state" );
			HUB().trigger<Events::UpdateDownloadFailed>( "Downloaded update is no longer available." );
			return;
		}

		HUB().trigger<Events::UpdateDownloadProgress>( 100 );

		_impl->updateDownloadInProgress = false;
		_impl->updateReadyToRestart		= true;
		HUB().trigger<Events::UpdateReadyToRestart>();
	}

	void Session::_onUpdateDownloadProgress( const Events::Update & )
	{
		if ( not _impl->updateDownloadInProgress )
		{
			return;
		}

		Util::Thread::BaseThread * downloadThread = THREAD().get( _impl->updateDownloadThreadId );
		if ( downloadThread == nullptr )
		{
			return;
		}

		const uint progress = uint( downloadThread->getProgress() * 100.f );
		HUB().trigger<Events::UpdateDownloadProgress>( progress );
	}

	void Session::applyDownloadedUpdate()
	{
		if ( not _impl->updateReadyToRestart.exchange( false ) )
		{
			VTX_TRACE( "applyDownloadedUpdate called without a downloaded update" );
			return;
		}

		if ( not _impl->manager || not _impl->pendingUpdate )
		{
			VTX_TRACE( "applyDownloadedUpdate aborted due to invalid updater state" );
			return;
		}

		try
		{
			const Velopack::UpdateInfo pendingUpdate = *_impl->pendingUpdate;
			std::vector<std::string>   args			 = toStringVector( ARGS() );
			( *_impl->manager )
				.WaitExitThenApplyUpdates( pendingUpdate, false, true, { args.begin() + 1, args.end() } );
			VTX_TRACE( "applyDownloadedUpdate: WaitExitThenApplyUpdates returned" );
			ACTION().execute<Action::Application::Quit>();
		}
		catch ( const std::exception & p_e )
		{
			_impl->updateReadyToRestart = true;
			VTX_ERROR( "Update apply error: {}", p_e.what() );
			HUB().trigger<Events::ApplicationError>( "Failed to restart and apply the downloaded update." );
		}
	}

	bool Session::hasManager() const { return _impl->manager.has_value(); }

	bool Session::isPortable() const
	{
		assert( _impl->manager );

		return ( *_impl->manager ).IsPortable();
	}

	namespace
	{
		enum class SessionPathRoot
		{
			Data,
			Pictures
		};

		FilePath _getDefaultBaseDir( const SessionPathRoot p_root )
		{
			switch ( p_root )
			{
			case SessionPathRoot::Data: return Filesystem::getDataHome();
			case SessionPathRoot::Pictures: return Filesystem::getPicturesFolder();
			}

			return Filesystem::getDataHome();
		}

		FilePath _getPortableWindowsRoot()
		{
			const FilePath executablePath = Filesystem::getExecutable();
			const FilePath executableDir  = executablePath.parent_path();
			const FilePath parentDir	  = executableDir.parent_path();

			if ( not parentDir.empty() )
			{
				return parentDir;
			}

			return executableDir;
		}
	} // namespace

	FilePath Session::getDataHome() const
	{
		if ( not hasManager() )
		{
			return getApplicationDir();
		}

#if defined( _WIN32 )
		if ( isPortable() )
		{
			return _getPortableWindowsRoot();
		}
#endif

		return _getDefaultBaseDir( SessionPathRoot::Data ) / APP_FOLDER_NAME;
	}

	FilePath Session::getPicturesFolder() const
	{
		if ( not hasManager() )
		{
			return getApplicationDir();
		}

#if defined( _WIN32 )
		if ( isPortable() )
		{
			return _getPortableWindowsRoot();
		}
#endif

		return _getDefaultBaseDir( SessionPathRoot::Pictures ) / APP_FOLDER_NAME;
	}

	FilePath Session::getApplicationDir() const { return Filesystem::getExecutableDir(); }

	FilePath Session::getAppTmpFolder() const { return Filesystem::getSystemTmpFolder() / APP_FOLDER_NAME; }

	FilePath Session::getDataDir() const { return getApplicationDir() / "data"; }

	FilePath Session::getDefaultUserData() const { return getApplicationDir() / "default_user_data"; }

	FilePath Session::getDefaultRepresentationsDir() const { return getDefaultUserData() / "representations"; }

	FilePath Session::getDefaultColorLayoutsDir() const { return getDefaultUserData() / "colors"; }

	FilePath Session::getDefaultEffectsDir() const { return getDefaultUserData() / "effects"; }

	FilePath Session::getDefaultHdriDir() const { return getDefaultUserData() / "hdri"; }

	FilePath Session::getDefaultMaterialsDir() const { return getDefaultUserData() / "materials"; }

	FilePath Session::getDefaultScriptsDir() const { return getDefaultUserData() / "scripts"; }

	FilePath Session::getLicenseFile() const { return getApplicationDir() / "license.txt"; }

	FilePath Session::getLogsDir() const { return getDataHome() / "logs"; }

	FilePath Session::getCacheDir() const { return getDataHome() / "cache"; }

	FilePath Session::getSnapshotsDir() const { return getPicturesFolder() / "snapshots"; }

	FilePath Session::getRepresentationsDir() const { return getDataHome() / "representations"; }

	FilePath Session::getColorLayoutsDir() const { return getDataHome() / "colors"; }

	FilePath Session::getEffectsDir() const { return getDataHome() / "effects"; }

	FilePath Session::getHdriDir() const { return getDataHome() / "hdri"; }

	FilePath Session::getMaterialsDir() const { return getDataHome() / "materials"; }

	FilePath Session::getScriptsDir() const { return getDataHome() / "scripts"; }

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
		VTX_DEBUG( "Executable dir: {}", getApplicationDir().string() );
		VTX_DEBUG( "Data home: {}", getDataHome().string() );
		VTX_DEBUG( "Pictures folder: {}", getPicturesFolder().string() );
		VTX_DEBUG( "Tmp folder: {}", getAppTmpFolder().string() );
	}

} // namespace VTX::App
