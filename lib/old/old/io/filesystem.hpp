#ifndef __VTX_IO_FILESYSTEM__
#define __VTX_IO_FILESYSTEM__

#include "struct/image_export.hpp"
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>

namespace VTX::IO::Filesystem
{
	static FilePath EXECUTABLE_ABSOLUTE_PATH = "";
	// JSon save fail when size > 192
	static const int		 MAX_FILE_LENGTH	  = 180;
	static const std::string REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

	enum class FILE_TYPE : int
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
		assert( EXECUTABLE_ABSOLUTE_PATH != "" );
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
	// static const FilePath SHADERS_DIR_SRC		  = FilePath( "../src/shader" );
	// static const FilePath DEFAULT_SAVE_FOLDER	  = FilePath( "../save" );
	// static const FilePath DEFAULT_MOLECULE_FOLDER = FilePath( "../data" );

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

	inline FILE_TYPE getFileTypeFromFilePath( const FilePath & p_path )
	{
		const FilePath extension = p_path.extension();

		if ( extension == "vtx" )
		{
			return FILE_TYPE::SCENE;
		}
		else if ( extension == "prm" || extension == "psf" )
		{
			return FILE_TYPE::CONFIGURATION;
		}
		else if ( extension == "cif" || extension == "cml" || extension == "cssr" || extension == "gro"
				  || extension == "mmcif" || extension == "mmtf" || extension == "mol2" || extension == "molden"
				  || extension == "pdb" || extension == "sdf" || extension == "smi" || extension == "mmtf"
				  || extension == "xyz" )
		{
			return FILE_TYPE::MOLECULE;
		}
		else if ( extension == "obj" )
		{
			return FILE_TYPE::MESH;
		}
		else if ( extension == "nc" || extension == "dcd" || extension == "lammpstrj" || extension == "arc"
				  || extension == "trr" || extension == "xtc" || extension == "tng" || extension == "trj" )
		{
			return FILE_TYPE::TRAJECTORY;
		}
		else
		{
			return FILE_TYPE::UNKNOWN;
		}
	}

	inline void fillFilepathPerMode( std::vector<FilePath>				  p_filepaths,
									 std::vector<std::vector<FilePath>> & p_filepathPerMode )
	{
		p_filepathPerMode.resize( int( FILE_TYPE::COUNT ) );

		for ( const FilePath & path : p_filepaths )
		{
			const FILE_TYPE filetype = getFileTypeFromFilePath( path );
			p_filepathPerMode[ int( filetype ) ].emplace_back( path );
		}
	}

	void generateUniqueFileName( FilePath & p_filePath )
	{
		uint counter = 2;

		FilePath newPath = p_filePath.stem().string();
		while ( std::filesystem::exists( newPath ) )
		{
			newPath = p_filePath.stem().string() + "_" + std::to_string( counter ) + p_filePath.extension().string();
			counter++;
		}

		std::filesystem::rename( p_filePath, newPath );
	}

	const FilePath getUniqueSnapshotsPath( const Struct::ImageExport::Format p_format )
	{
		std::string extension;

		switch ( p_format )
		{
		case IO::Struct::ImageExport::Format::PNG: extension = "png"; break;
		case IO::Struct::ImageExport::Format::JPEG: extension = "jpg"; break;
		case IO::Struct::ImageExport::Format::BMP: extension = "bmp"; break;
		default:
			throw IOException( "Unknown format for snapshot (" + std::to_string( int( p_format ) ) );
			extension = "";
			break;
		}

		const std::string filename = std::to_string( Util::Chrono::getTimestamp() ) + '.' + extension;
		std::filesystem::create_directory( getSnapshotsDir() );

		FilePath path = FilePath( getSnapshotsDir() / filename );
		generateUniqueFileName( path );

		return path;
	}
} // namespace VTX::IO::Filesystem

#endif
