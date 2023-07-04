#ifndef __VTX_IO_INTERNAL_FILESYSTEM__
#define __VTX_IO_INTERNAL_FILESYSTEM__

#include <string>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::IO::Internal::Filesystem
{
	static FilePath EXECUTABLE_ABSOLUTE_PATH = std::filesystem::current_path();
	// JSon save fail when size > 192
	static const int		 MAX_FILE_LENGTH	  = 180;
	static const std::string REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

	enum class FILE_TYPE_ENUM : int
	{
		SCENE,
		CONFIGURATION,
		MOLECULE,
		TRAJECTORY,
		MESH,
		UNKNOWN,
		COUNT
	};

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
	inline const FilePath getLibrariesDir() { return getExecutableDir() / "libraries"; }
	inline const FilePath getRepresentationsLibraryDir() { return getLibrariesDir() / "representations"; }
	inline const FilePath getRenderEffectPresetsLibraryDir() { return getLibrariesDir() / "render_effects"; }
	inline const FilePath getInternalDataDir() { return getExecutableDir() / "data"; }
	inline const FilePath getResidueDataDir() { return getInternalDataDir() / "residue_data"; }

	// Files.
	inline const FilePath getConfigIniFile() { return getExecutableDir() / "config.ini"; }
	inline const FilePath getSettingJsonFile() { return getExecutableDir() / "setting.json"; }
	inline const FilePath getLicenseFile() { return getExecutableDir() / "license.txt"; }

	// Dev directories.
	static const FilePath SHADERS_DIR_SRC		  = FilePath( "../src/shader" );
	static const FilePath DEFAULT_SAVE_FOLDER	  = FilePath( "../save" );
	static const FilePath DEFAULT_MOLECULE_FOLDER = FilePath( "../data" );

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

	inline FILE_TYPE_ENUM getFileTypeFromFilePath( const FilePath & p_path )
	{
		const FilePath extension = p_path.extension();

		if ( extension == ".vtx" )
		{
			return FILE_TYPE_ENUM::SCENE;
		}
		else if ( extension == ".prm" || extension == ".psf" )
		{
			return FILE_TYPE_ENUM::CONFIGURATION;
		}
		else if ( extension == ".cif" || extension == ".cml" || extension == ".cssr" || extension == ".gro"
				  || extension == ".mmcif" || extension == ".mmtf" || extension == ".mol2" || extension == ".molden"
				  || extension == ".pdb" || extension == ".sdf" || extension == ".smi" || extension == ".mmtf"
				  || extension == ".xyz" )
		{
			return FILE_TYPE_ENUM::MOLECULE;
		}
		else if ( extension == ".obj" )
		{
			return FILE_TYPE_ENUM::MESH;
		}
		else if ( extension == ".nc" || extension == ".dcd" || extension == ".lammpstrj" || extension == ".arc"
				  || extension == ".trr" || extension == ".xtc" || extension == ".tng" || extension == ".trj" )
		{
			return FILE_TYPE_ENUM::TRAJECTORY;
		}
		else
		{
			return FILE_TYPE_ENUM::UNKNOWN;
		}
	}

	inline void fillFilepathPerMode( std::vector<FilePath>				  p_filepaths,
									 std::vector<std::vector<FilePath>> & p_filepathPerMode )
	{
		p_filepathPerMode.resize( int( FILE_TYPE_ENUM::COUNT ) );

		for ( const FilePath & path : p_filepaths )
		{
			const FILE_TYPE_ENUM filetype = getFileTypeFromFilePath( path );
			p_filepathPerMode[ int( filetype ) ].emplace_back( path );
		}
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
} // namespace VTX::IO::Internal::Filesystem

#endif
