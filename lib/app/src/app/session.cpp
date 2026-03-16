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
		/*
		Events::UpdateAvailable e;
		e.currentVersion = version();
		e.newVersion	 = "2026.3.0";
		e.changelogHtml
			= "<h1>Change Log</h1>\n<h2>[0.4.2] (beta) - 2024-02-29</h2>\n<h3>Debug</h3>\n<ul>\n<li>Fix possible crash "
			  "when loading big trajectory.</li>\n<li>Fix Memory leak when reading trajectory in SES "
			  "representation.</li>\n<li>Debug crazy camera movement after focusing render "
			  "panel.</li>\n</ul>\n<h3>UI</h3>\n<ul>\n<li>Display warning pop-up when trying to compute huge "
			  "SES.</li>\n<li>Add feedback when loading a molecule.</li>\n<li>Display full path to image file when "
			  "taking instant snapshot.</li>\n<li>Rename color modes Atom-Chain, Atom-Molecule, Atom-Custom into "
			  "CHNOPS Chain, CHNOPS Molecule and CHNOPS Custom.</li>\n<li>Fix some typo.</li>\n</ul>\n<h2>[0.4.1] "
			  "(beta) - 2023-05-30</h2>\n<h3>Debug</h3>\n<ul>\n<li>Fix issue with snapshot on scaled "
			  "screens.</li>\n<li>Fix transparent outline on snapshots.</li>\n<li>Debug display of Position section of "
			  "atom Inspector.</li>\n<li>Debug issue with Disulfide bond guessing on .cif files.</li>\n<li>Debug right "
			  "click on Overlay which was applied on render.</li>\n</ul>\n<h3>Optimizations</h3>\n<ul>\n<li>Optimize "
			  "duplicate frame submenu.</li>\n<li>Optimize selection.</li>\n</ul>\n<h3>UI</h3>\n<ul>\n<li>Improve "
			  "resize area of docked panels.</li>\n</ul>\n<h3>Other</h3>\n<ul>\n<li>Update links in information to the "
			  "new VTX git repository.</li>\n</ul>\n<h2>[0.4.0] (beta) - 2023-05-04</h2>\n<h3>New "
			  "features</h3>\n<ul>\n<li>Add SES representation for molecules.</li>\n<li>Add an orthographic "
			  "camera.</li>\n<li>Add functionality to compute RMSD and Structural Alignments on "
			  "molecules.</li>\n<li>Add a layer called Category in the molecule hierarchy to group chains that contain "
			  "the same type of residues (Categories are Polymer, Carbohydrate, Ligand, Ion, Solvent and "
			  "Water).</li>\n<li>Add ability to load a trajectory file on a loaded molecule.</li>\n<li>Add a "
			  "functionality to duplicate a single frame of a dynamic in a new "
			  "molecule.</li>\n</ul>\n<h3>Debug</h3>\n<ul>\n<li>Debug issues with extract and duplicate "
			  "functions.</li>\n<li>Debug issues with molecule export.</li>\n<li>Debug issues with sequence "
			  "window.</li>\n<li>Debug transform modification when multiple molecules were "
			  "selected.</li>\n</ul>\n<h3>Optimizations</h3>\n<ul>\n<li>Computation of SS (and SES) done when needed "
			  "instead of compute it at molecule loading.</li>\n<li>Optimization of Show/Hide "
			  "functions.</li>\n<li>Optimization of sequence generation for molecules with numerous unknown "
			  "residues.</li>\n<li>Add a menu to display a specific inspector in the molecule hierarchy (i.e. display "
			  "the inspector of all chains instead of the inspector of the molecule when the molecule is fully "
			  "selected)</li>\n</ul>\n<h3>UI</h3>\n<ul>\n<li>Add a quick access to render effect presets in the render "
			  "view.</li>\n<li>Visualization tab in main menu splitted into Camera and Molecule tabs.</li>\n<li>Export "
			  "Molecule button duplicated in the Home tab.</li>\n<li>Add a quick access to background color in the "
			  "main menu.</li>\n<li>Move camera parameters from Render Effects settings to VTX "
			  "settings.</li>\n<li>Update Settings window layout.</li>\n<li>Display representations of children in the "
			  "representation inspector.</li>\n<li>Display more data in the information section of the chain "
			  "inspector.</li>\n<li>Limits number of bonds displayed in the inspector to "
			  "100.</li>\n</ul>\n<h3>Other</h3>\n<ul>\n<li>Update chemfiles to version 0.10.3.</li>\n<li>Split "
			  "translation speed factor into acceleration and deceleration speed factor for camera "
			  "movements.</li>\n</ul>\n<h2>[0.3.0] (beta) - 2022-05-23</h2>\n<h3>New features</h3>\n<ul>\n<li>Measure "
			  "tool.</li>\n<li>Freeze inspector button.</li>\n<li>Add color blending mode in "
			  "representations.</li>\n<li>Add contextual menu on viewpoint container.</li>\n<li>Manage &quot;Go "
			  "to&quot; function of viewpoints with multiple selection.</li>\n</ul>\n<h3>Debug</h3>\n<ul>\n<li>Debug "
			  "preview opacity when exporting an image.</li>\n<li>Debug chain / residue / atom count update in "
			  "molecule inspector after a modification in the molecule structure.</li>\n<li>Improve stability after an "
			  "extract.</li>\n<li>Debug orient shortcut without selection.</li>\n<li>Applying a representation without "
			  "selection apply it on every molecule instead of the first one only.</li>\n<li>Debug camera clip refresh "
			  "after a reset.</li>\n<li>Debug picking on scaled screens.</li>\n<li>Debug negative scale using drag on "
			  "transform widget.</li>\n</ul>\n<h3>Other</h3>\n<ul>\n<li>Upgrade interface library to "
			  "Qt6.</li>\n<li>Rearrange visualization buttons in main menu.</li>\n<li>Improve widget to set fog clip "
			  "and camera clip in render effects.</li>\n</ul>\n<h2>[0.2.0] (beta) - 2021-12-13</h2>\n<ul>\n<li>Picking "
			  "(selection in 3D)</li>\n<li>Advanced image exporter</li>\n<li>Creation of Viewpoints to record a camera "
			  "point of view</li>\n<li>Trajectory can be open directly on a molecule</li>\n<li>Can open files or "
			  "download molecules in command line</li>\n<li>New visibility actions :\n<ul>\n<li>&quot;Solo&quot; "
			  "action to hide everything else than the target</li>\n<li>&quot;Show All&quot; action to show every "
			  "objects in the scene</li>\n</ul>\n</li>\n<li>Add atom name in atom's inspector</li>\n<li>Save and "
			  "library files are now cross-plateform</li>\n<li>Export and Fullscreen buttons has been relocated in the "
			  "main menu</li>\n<li>Library reloading debugged</li>\n<li>Selection optimized</li>\n<li>New UI toolbar "
			  "overlay</li>\n</ul>\n<h2>[0.1.2] (beta) - 2021-10-20</h2>\n<ul>\n<li>Check for an available update at "
			  "startup</li>\n<li>Read mmCIF assemblies</li>\n<li>Main menu improved. Molecule section has been "
			  "removed, &quot;Download&quot; action has moved to &quot;File&quot; section and &quot;Export&quot; "
			  "action to &quot;Selection Action&quot; section</li>\n<li>Portable save option added (the save may be "
			  "longer but can be shared on different devices)</li>\n<li>MMCIF parser "
			  "optimization</li>\n</ul>\n<h2>[0.1.1] (beta) - 2021-09-29</h2>\n<p>The first release of VTX.\nFeel free "
			  "to give your feedback in the public <a href=\"https://gitlab.com/VTX_mol/VTX/-/issues\">issue "
			  "tracker</a>.</p>";
		e.size = 2048;
		HUB().trigger<Events::UpdateAvailable>( e );
		*/
		/////////////////////////////////////////
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
