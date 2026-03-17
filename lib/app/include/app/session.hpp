#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <memory>
#include <optional>
#include <util/types.hpp>

namespace VTX::App
{
	// Resolved in .cpp with preprocessor definitions only passed to App.
	extern const uint VERSION_MAJOR;
	extern const uint VERSION_MINOR;
	extern const uint VERSION_PATCH;

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX Team";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";

	constexpr std::string_view UPDATE_URL
		= "https://github.com/VTX-Molecular-Visualization/VTX_release/releases/latest/download";
	constexpr std::string_view APP_FOLDER_NAME = "VTX";

	/**
	 * @brief Runtime session.
	 */
	class Session
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Session();
		~Session();

		/**
		 * @brief Current version.
		 */
		inline std::string version() const
		{
			return std::to_string( VERSION_MAJOR ) + "." + std::to_string( VERSION_MINOR ) + "."
				   + std::to_string( VERSION_PATCH );
		}

		/**
		 * @brief Check for application update.
		 */
		void checkForUpdate();

		/**
		 * @brief Download the latest update.
		 */
		void downloadUpdate();

		/**
		 * @brief Is this version portable.
		 */
		bool isPortable() const;

		/**
		 * @brief Get application directories/files.
		 */
		FilePath getDataHome() const;
		FilePath getPicturesFolder() const;

		FilePath getShadersDir() const;
		FilePath getLicenseFile() const;
		FilePath getReadmeFile() const;
		FilePath getChangelogFile() const;
		FilePath getDataDir() const;
		FilePath getResidueDataDir() const;
		FilePath getResidueDataFilePath( const std::string_view );
		FilePath getLogsDir() const;
		FilePath getCacheDir() const;
		FilePath getSnapshotsDir() const;
		FilePath getRepresentationsDir() const;
		FilePath getColorLayoutsDir() const;
		FilePath getEffectsDir() const;
		FilePath getConfigIniFile() const;
		// const FilePath getSettingJsonFile() const;

		/**
		 * @brief Print session information (for debug).
		 */
		void print() const;

		// TODO: clean old:
		/*
		inline bool isSessionFile( const FilePath & p_filePath ) { return p_filePath.extension().string() == "vtx"; }

		inline FilePath getSceneSaveDirectory( const FilePath & p_savePath )
		{
			const FilePath projectDirectoryName = FilePath( p_savePath.stem().append( "_data" ) );

			return p_savePath.parent_path() / projectDirectoryName;
		}

		inline FilePath getSceneObjectsSaveDirectory( const FilePath & p_savePath )
		{
			return getSceneSaveDirectory( p_savePath ) / "obj";
		}

		inline FilePath getResidueDataFilePath( const std::string & p_residueName )
		{
			return getResidueDataDir() / p_residueName.substr( 0, 1 );
		}

		inline void checkSaveDirectoryHierarchy( const FilePath & p_savePath )
		{
			const FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
			if ( std::filesystem::exists( projectDirectory ) == false )
			{
				std::filesystem::create_directory( projectDirectory );
			}

			const FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
			if ( std::filesystem::exists( objectsPath ) == false )
			{
				std::filesystem::create_directory( objectsPath );
			}
		}
		*/

	  private:
		/**
		 * @brief PImpl.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
} // namespace VTX::App

#endif
