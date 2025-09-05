#ifndef __VTX_APP_FILESYSTEM__
#define __VTX_APP_FILESYSTEM__

#include <string>
#include <util/filesystem.hpp>

namespace VTX::App::Filesystem
{

	inline const FilePath EXECUTABLE_DIR = VTX::Util::Filesystem::getExecutableDir();
	inline const FilePath USER_DATA_DIR	 = VTX::Util::Filesystem::getUserDataDir();

	// System.
	inline const FilePath getShadersDir() { return EXECUTABLE_DIR / "shaders"; }
	inline const FilePath getDataDir() { return EXECUTABLE_DIR / "data"; }
	inline const FilePath getResidueDataDir() { return getDataDir() / "residue"; }

	inline const FilePath getLicenseFile() { return EXECUTABLE_DIR / "license.txt"; }

	// User.
	inline const FilePath getLogsDir() { return USER_DATA_DIR / "logs"; }
	inline const FilePath getCacheDir() { return USER_DATA_DIR / "cache"; }
	inline const FilePath getSnapshotsDir() { return USER_DATA_DIR / "snapshots"; }
	inline const FilePath getRepresentationsDir() { return USER_DATA_DIR / "representations"; }
	inline const FilePath getColorLayoutsDir() { return USER_DATA_DIR / "colors"; }
	inline const FilePath getEffectsDir() { return USER_DATA_DIR / "effects"; }

	inline const FilePath getConfigIniFile() { return USER_DATA_DIR / "config.ini"; }
	inline const FilePath getSettingJsonFile() { return USER_DATA_DIR / "setting.json"; }

	// TODO: clean old:

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
} // namespace VTX::App::Filesystem

#endif
