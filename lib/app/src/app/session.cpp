#include "app/session.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <velopack/include/Velopack.hpp>

using namespace VTX::Util;

namespace VTX::App
{
	const uint VERSION_MAJOR = static_cast<uint>( VTX_VERSION_MAJOR );
	const uint VERSION_MINOR = static_cast<uint>( VTX_VERSION_MINOR );
	const uint VERSION_PATCH = static_cast<uint>( VTX_VERSION_PATCH );

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
	};

	Session::Session() : _impl( std::make_unique<Impl>() )
	{
		try
		{
			// Velopack hooks and run.
			auto vpCallback = []( void * p_user_data, const char * psz_app_version ) {};
			Velopack::VelopackApp::Build()
				.OnAfterInstall( vpCallback )
				.OnAfterInstall( vpCallback )
				.OnBeforeUninstall( vpCallback )
				.OnBeforeUpdate( vpCallback )
				.OnAfterUpdate( vpCallback )
				.OnFirstRun( vpCallback )
				.OnRestarted( vpCallback )
				.Run();

			_impl->manager.emplace( UPDATE_URL.data() );
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

		try
		{
			auto update = ( *_impl->manager ).CheckForUpdates();
			if ( update.has_value() )
			{
				_impl->pendingUpdate = std::move( update );
				const auto & release = _impl->pendingUpdate->TargetFullRelease;
				VTX_INFO( "New version found: {}", release.Version );
				HUB().trigger<Events::UpdateAvailable>(
					version(), release.Version, release.NotesMarkdown, release.NotesHtml, release.Size
				);
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

	void Session::downloadUpdate()
	{
		assert( _impl->pendingUpdate );

		( *_impl->manager ).DownloadUpdates( *_impl->pendingUpdate );
		( *_impl->manager ).WaitExitThenApplyUpdates( *_impl->pendingUpdate );
	}

	bool Session::isPortable() const
	{
		if ( not _impl->manager )
		{
			return true;
		}
		return ( *_impl->manager ).IsPortable();
	}

	FilePath Session::getDataHome() const
	{
		if ( isPortable() )
		{
			return Filesystem::getExecutableDir();
		}
		return Filesystem::getDataHome() / "VTX";
	}

	FilePath Session::getPicturesFolder() const
	{
		if ( isPortable() )
		{
			return Filesystem::getExecutableDir();
		}
		return Filesystem::getPicturesFolder() / "VTX";
	}

	FilePath Session::getShadersDir() const { return Filesystem::getExecutableDir() / "shaders"; }
	FilePath Session::getLicenseFile() const { return Filesystem::getExecutableDir() / "license.txt"; }
	FilePath Session::getReadmeFile() const { return Filesystem::getExecutableDir() / "README.md"; }
	FilePath Session::getChangelogFile() const { return Filesystem::getExecutableDir() / "CHANGELOG.md"; }
	FilePath Session::getResidueDataDir() const { return Filesystem::getExecutableDir() / "residue"; }
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
