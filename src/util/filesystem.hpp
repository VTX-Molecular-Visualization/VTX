#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "../exception.hpp"
#include "define.hpp"
#include "tool/logger.hpp"
#include <QString>
#include <fstream>
#include <set>

#ifdef _MSC_VER
#include <Windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			inline const FilePath getExecutableFile()
			{
#ifdef _MSC_VER
				wchar_t path[ MAX_PATH ] = { 0 };
				GetModuleFileNameW( NULL, path, MAX_PATH );
				return FilePath( path );
#else
				char rawPathName[ PATH_MAX ];
				realpath( "/proc/self/exe", rawPathName );
				return Path( rawPathName );
#endif
			}

			// JSon save fail when size > 192
			static const int		 MAX_FILE_LENGTH	  = 180;
			static const std::string REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

			static const FilePath EXECUTABLE_FILE = getExecutableFile();
#ifdef _MSC_VER
			static const FilePath EXECUTABLE_DIR = EXECUTABLE_FILE.parent_path().parent_path().parent_path();
#else
			static const Path EXECUTABLE_DIR = EXECUTABLE_FILE.parent_path().parent_path();
#endif

			static const FilePath SHADERS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/shaders" );
			static const FilePath SNAPSHOTS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/snapshots" );
			static const FilePath RENDERS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/renders" );
			static const FilePath PATHS_DIR			= FilePath( EXECUTABLE_DIR.string() + "/paths" );
			static const FilePath VIDEOS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/videos" );
			static const FilePath LOGS_DIR			= FilePath( EXECUTABLE_DIR.string() + "/logs" );
			static const FilePath LIBRARIES_DIR		= FilePath( EXECUTABLE_DIR.string() + "/libraries" );
			static const FilePath INTERNAL_DATA_DIR = FilePath( EXECUTABLE_DIR.string() + "/data" );

			static const FilePath SCENE_OBJECT_FOLDER = FilePath( "obj" );

			static const FilePath LICENSE_PATH			 = FilePath( EXECUTABLE_DIR.string() + "/license.txt" );
			static const FilePath RESIDUE_DATA_DIRECTORY = INTERNAL_DATA_DIR / "residue_data";

			static const FilePath DATA_DIR				  = "../data";
			static const FilePath SHADERS_DIR_SRC		  = "../src/shader";
			static const QString  DEFAULT_SAVE_FOLDER	  = "../save";
			static const QString  DEFAULT_MOLECULE_FOLDER = "../data";

			static const QString MOLECULE_EXTENSIONS_READ
				= "Amber Net CDF (*.nc);;"
				  "CIF(*.cif );;"
				  "CML (*.cml);;"
				  "CSSR (*.cssr);;"
				  "DCD (*.dcd);;"
				  "GRO (*.gro);;"
				  "LAMMPS (*.lammpstrj);;"
				  "MMCIF(*.mmcif);;"
				  "MMTF (*.mmtf);;"
				  "MOL2 (*.mol2);;"
				  "Molden (*.molden);;"
				  "PDB (*.pdb);;"
				  "SDF (*.sdf);;"
				  "SMI (*.smi);;"
				  "Tinker (*.arc *.psf *.prm);;"
				  "TNG (*.tng);;"
				  "TRJ (*.trj);;"
				  "TRR (*.trr);;"
				  "XTC (*.xtc);;"
				  "XYZ (*.xyz);;"
				  "All (*)";

			static const QString MOLECULE_EXTENSIONS_WRITE
				= "Amber Net CDF (*.nc);;"
				  "CIF(*.cif );;"
				  "CML (*.cml);;"
				  "CSSR (*.cssr);;"
				  "GRO (*.gro);;"
				  "LAMMPS (*.lammpstrj);;"
				  "MMCIF(*.mmcif);;"
				  "MMTF (*.mmtf);;"
				  "MOL2 (*.mol2);;"
				  "PDB (*.pdb);;"
				  "SDF (*.sdf);;"
				  "SMI (*.smi);;"
				  "Tinker (*.arc *.psf *.prm);;"
				  "TRR (*.trr);;"
				  "XTC (*.xtc);;"
				  "XYZ (*.xyz);;";

			static const QString VTX_EXTENSIONS = "VTX file (*.vtx)";

			static const QString LOAD_MOLECULE_FILTERS	 = MOLECULE_EXTENSIONS_READ;
			static const QString EXPORT_MOLECULE_FILTERS = MOLECULE_EXTENSIONS_WRITE;
			static const QString OPEN_FILE_FILTERS		 = VTX_EXTENSIONS + ";;" + MOLECULE_EXTENSIONS_READ;
			static const QString SAVE_SCENE_FILTERS		 = VTX_EXTENSIONS;

			static const QString DEFAULT_MOLECULE_READ_FILTER	  = "All (*)";
			static const QString DEFAULT_MOLECULE_WRITE_FILTER	  = "MMCIF(*.mmcif)";
			static const QString DEFAULT_MOLECULE_WRITE_EXTENSION = DEFAULT_MOLECULE_WRITE_FILTER.mid(
				DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) + 1,
				DEFAULT_MOLECULE_WRITE_FILTER.size() - DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) - 2 );

			static const QString DEFAULT_FILE_READ_FILTER  = "All (*)";
			static const QString DEFAULT_FILE_WRITE_FILTER = VTX_EXTENSIONS;

			static const QString REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
			static const QString RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

			static const FilePath REPRESENTATION_LIBRARY_DIR = FilePath( LIBRARIES_DIR.string() + "/representations" );
			static const FilePath RENDER_EFFECT_PRESET_LIBRARY_DIR
				= FilePath( LIBRARIES_DIR.string() + "/render_effects" );

			static const FilePath SETTING_JSON_FILE = FilePath( EXECUTABLE_DIR.string() + "/setting.json" );
			static const FilePath CONFIG_INI_FILE	= FilePath( EXECUTABLE_DIR.string() + "/config.ini" );

			static const FilePath FFMPEG_EXE_FILE = FilePath( "bin/ffmpeg.exe" );

			static const char * STYLESHEET_FILE_DEFAULT = ":/stylesheet.css";

			static const std::string DEFAULT_SCENE_FILENAME	   = "New Scene";
			static const std::string DEFAULT_MOLECULE_FILENAME = "New Molecule";

			inline const FilePath getDataPath( const std::string & p_filename )
			{
				return FilePath( DATA_DIR / p_filename );
			}

			inline const FilePath getShadersPath( const std::string & p_filename )
			{
				return FilePath( SHADERS_DIR ) /= p_filename;
			}

			inline const FilePath getSnapshotsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( SNAPSHOTS_DIR );
				return FilePath( SNAPSHOTS_DIR ) /= p_filename;
			}

			inline const FilePath getRendersPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( RENDERS_DIR );
				return FilePath( RENDERS_DIR ) /= p_filename;
			}

			inline const FilePath getPathsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( PATHS_DIR );
				return FilePath( PATHS_DIR ) /= p_filename;
			}

			inline const FilePath getVideosBatchPath( const std::string & p_batchName )
			{
				FilePath dir = FilePath( VIDEOS_DIR ) /= p_batchName;
				std::filesystem::create_directories( dir );
				return FilePath( dir );
			}

			inline const FilePath getVideosPath( const std::string & p_batchName, const std::string & p_fileName )
			{
				FilePath dir = getVideosBatchPath( p_batchName );
				return dir /= p_fileName;
			}

			inline const FilePath getVideosPath( const std::string & p_fileName )
			{
				return FilePath( VIDEOS_DIR ) /= p_fileName;
			}

			inline const FilePath getLogsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( LOGS_DIR );
				return FilePath( LOGS_DIR ) /= p_filename;
			}

			inline const FilePath getRepresentationLibraryDirectory()
			{
				std::filesystem::create_directories( LIBRARIES_DIR );
				std::filesystem::create_directories( REPRESENTATION_LIBRARY_DIR );

				return REPRESENTATION_LIBRARY_DIR;
			}
			inline const FilePath getRepresentationPath( const std::string & p_filename )
			{
				return getRepresentationLibraryDirectory() / p_filename;
			}

			inline const FilePath getRenderEffectPresetLibraryDirectory()
			{
				std::filesystem::create_directories( LIBRARIES_DIR );
				std::filesystem::create_directories( RENDER_EFFECT_PRESET_LIBRARY_DIR );

				return RENDER_EFFECT_PRESET_LIBRARY_DIR;
			}
			inline const FilePath getRenderEffectPath( const std::string & p_filename )
			{
				return getRenderEffectPresetLibraryDirectory() / p_filename;
			}
			inline bool isSessionFile( const FilePath & p_filePath ) { return p_filePath.extension() == ".vtx"; }

			inline const std::string readPath( const FilePath & p_path )
			{
				std::ifstream file;
				file.open( p_path, std::ios::in );

				if ( !file.is_open() )
				{
					throw Exception::IOException( "Cannot open file " + p_path.string() );
				}

				const uintmax_t size = std::filesystem::file_size( p_path );
				std::string		result( size, '\0' );
				file.read( result.data(), size );
				file.close();

				return result;
			}

			inline void readPath( const FilePath & p_path, std::string & p_content )
			{
				std::ifstream file;
				file.open( p_path, std::ios::in );

				if ( !file.is_open() )
				{
					throw Exception::IOException( "Cannot open file " + p_path.string() );
				}

				const uintmax_t size = std::filesystem::file_size( p_path );
				p_content.resize( size, '\0' );
				file.read( p_content.data(), size );
				file.close();
			}

			inline void		createDirectory( const FilePath & p_path ) { std::filesystem::create_directory( p_path ); }
			inline FilePath getParentPath( const FilePath & p_path ) { return p_path.parent_path(); }

			inline FilePath getFileNameWithoutExtension( const FilePath & p_path )
			{
				return p_path.filename().replace_extension();
			}

			inline void generateUniqueFileName( FilePath & p_path )
			{
				uint counter = 2;

				const std::string defaultFileName = getFileNameWithoutExtension( p_path ).string();
				const std::string extension		  = p_path.extension().string();

				while ( std::filesystem::exists( p_path ) )
				{
					p_path.replace_filename( defaultFileName + " " + std::to_string( counter ) + extension );
					counter++;
				}
			}

			inline bool copy( const FilePath & p_from, const FilePath & p_to )
			{
				bool succeed;

				try
				{
					std::filesystem::copy( p_from, p_to, std::filesystem::copy_options::recursive );
					succeed = true;
				}
				catch ( std::exception e )
				{
					VTX_WARNING( "Cannot copy file " + p_from.string() + " to " + p_to.string() + " : " + e.what() );
					succeed = false;
				}

				return succeed;
			}

			inline bool removeAll( const FilePath & p_directory )
			{
				bool succeed;
				try
				{
					succeed = std::filesystem::remove_all( p_directory );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Error when clear directory " + p_directory.string() + " : " + e.what() );
					succeed = false;
				}

				return succeed;
			}

			inline void getFilesInDirectory( const FilePath & p_directory, std::set<std::string> & p_container )
			{
				for ( const FilePath & file : std::filesystem::directory_iterator( p_directory ) )
					p_container.emplace( file.string() );
			}

			inline bool remove( const FilePath & p_filename )
			{
				bool succeed;
				try
				{
					succeed = std::filesystem::remove( p_filename );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Error when removing file " + p_filename.string() + " : " + e.what() );
					succeed = false;
				}

				return succeed;
			}

			inline bool exists( const FilePath & p_path ) { return std::filesystem::exists( p_path ); }

			inline FilePath getSceneSaveDirectory( const FilePath & p_savePath )
			{
				const FilePath projectDirectoryName
					= FilePath( getFileNameWithoutExtension( p_savePath ).string() + "_Data" );

				return getParentPath( p_savePath ) / projectDirectoryName;
			}
			inline FilePath getSceneObjectsSaveDirectory( const FilePath & p_savePath )
			{
				const FilePath projectDirectoryName
					= FilePath( getFileNameWithoutExtension( p_savePath ).string() + "_Data" );

				return getSceneSaveDirectory( p_savePath ) / SCENE_OBJECT_FOLDER;
			}

			inline void checkSaveDirectoryHierarchy( const FilePath & p_savePath )
			{
				const FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
				if ( !Util::Filesystem::exists( projectDirectory ) )
					createDirectory( projectDirectory );

				const FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
				if ( !Util::Filesystem::exists( objectsPath ) )
					createDirectory( objectsPath );
			}

			inline FilePath getResidueDataFilePath( const std::string & p_residueName )
			{
				return RESIDUE_DATA_DIRECTORY / p_residueName.substr( 0, 1 );
			}

			FilePath getDefaultMoleculeExportPath();
			FilePath getDefaultSceneSavePath();

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
