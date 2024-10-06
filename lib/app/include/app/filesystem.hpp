#ifndef __VTX_APP_FILESYSTEM__
#define __VTX_APP_FILESYSTEM__

#include <string>
#include <util/filesystem.hpp>

namespace VTX::App::Filesystem
{

	inline const FilePath EXECUTABLE_ABSOLUTE_PATH = VTX::Util::Filesystem::getExecutableDir();

	inline const FilePath getExecutableDir()
	{
		assert( std::filesystem::exists( EXECUTABLE_ABSOLUTE_PATH ) );
		return EXECUTABLE_ABSOLUTE_PATH;
	}

	// Directories.
	inline const FilePath getShadersDir() { return getExecutableDir() / "shaders"; }
	inline const FilePath getSnapshotsDir() { return getExecutableDir() / "snapshots"; }
	inline const FilePath getRendersDir() { return getExecutableDir() / "renders"; }
	inline const FilePath getLogsDir() { return getExecutableDir() / "logs"; }
	inline const FilePath getCacheDir() { return getExecutableDir() / "cache"; }
	inline const FilePath getLibrariesDir() { return getExecutableDir() / "libraries"; }
	inline const FilePath getRepresentationsLibraryDir() { return getLibrariesDir() / "representations"; }
	inline const FilePath getRenderEffectPresetsLibraryDir() { return getLibrariesDir() / "render_effects"; }
	inline const FilePath getThemesLibraryDir() { return getLibrariesDir() / "themes"; }
	inline const FilePath getInternalDataDir() { return getExecutableDir() / "data"; }
	inline const FilePath getResidueDataDir() { return getInternalDataDir() / "residue_data"; }

	// Files.
	inline const FilePath getConfigIniFile() { return getExecutableDir() / "config.ini"; }
	inline const FilePath getSettingJsonFile() { return getExecutableDir() / "setting.json"; }
	inline const FilePath getLicenseFile() { return getExecutableDir() / "license.txt"; }

	// Dev directories.
	// static const FilePath SHADERS_DIR_SRC		  = FilePath( "../src/shader" );
	inline const FilePath DEFAULT_SAVE_FOLDER	  = FilePath( "../save" );
	inline const FilePath DEFAULT_MOLECULE_FOLDER = FilePath( "../data" );

	inline const FilePath getShadersPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getShadersDir() );
		return FilePath( getShadersDir() / p_filename );
	}

	inline const FilePath getSnapshotsPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getSnapshotsDir() );
		return FilePath( getSnapshotsDir() / p_filename );
	}

	inline const FilePath getRendersPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRendersDir() );
		return FilePath( getRendersDir() / p_filename );
	}

	inline const FilePath getLogsPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getLogsDir() );
		return FilePath( getLogsDir() / p_filename );
	}

	inline const FilePath getCachePath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getCacheDir() );
		return FilePath( getCacheDir() / p_filename );
	}

	inline const FilePath getRepresentationPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRepresentationsLibraryDir() );
		return FilePath( getRepresentationsLibraryDir() / p_filename );
	}

	inline const FilePath getRenderEffectPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRenderEffectPresetsLibraryDir() );
		return FilePath( getRenderEffectPresetsLibraryDir() / p_filename );
	}

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
