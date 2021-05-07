#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "../exception.hpp"
#include "define.hpp"
#include "tool/logger.hpp"
#include <QString>
#include <fstream>

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
			// Test only.
			static const FilePath DATA_DIR = "../data";

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

			static const FilePath EXECUTABLE_FILE = getExecutableFile();
#ifdef _MSC_VER
			static const FilePath EXECUTABLE_DIR = EXECUTABLE_FILE.parent_path().parent_path().parent_path();
#else
			static const Path EXECUTABLE_DIR = EXECUTABLE_FILE.parent_path().parent_path();
#endif

			static const FilePath SHADERS_DIR	= FilePath( EXECUTABLE_DIR.string() + "/shaders" );
			static const FilePath SNAPSHOTS_DIR = FilePath( EXECUTABLE_DIR.string() + "/snapshots" );
			static const FilePath RENDERS_DIR	= FilePath( EXECUTABLE_DIR.string() + "/renders" );
			static const FilePath PATHS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/paths" );
			static const FilePath VIDEOS_DIR	= FilePath( EXECUTABLE_DIR.string() + "/videos" );
			static const FilePath LOGS_DIR		= FilePath( EXECUTABLE_DIR.string() + "/logs" );
			static const FilePath LIBRARIES_DIR = FilePath( EXECUTABLE_DIR.string() + "/libraries" );

			static const QString DEFAULT_SAVE_FOLDER	 = "../save";
			static const QString DEFAULT_MOLECULE_FOLDER = "../data";

			static const QString MOLECULE_EXTENSIONS = "*.pdb *.cif *.mmtf *.mol2 *.arc *.psf *.prm";
			static const QString VTX_EXTENSIONS		 = "*.vtx";

			static const QString MOLECULE_FILE_FILTERS = "Molecule file (" + MOLECULE_EXTENSIONS + ")";
			static const QString OPEN_FILE_FILTERS = "VTX file (" + VTX_EXTENSIONS + " " + MOLECULE_EXTENSIONS + ")";
			static const QString SAVE_FILE_FILTERS = "VTX file (" + VTX_EXTENSIONS + ")";

			static const QString REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
			static const QString RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

			static const FilePath REPRESENTATION_LIBRARY_DIR = FilePath( LIBRARIES_DIR.string() + "/representations" );
			static const FilePath RENDER_EFFECT_PRESET_LIBRARY_DIR	 = FilePath( LIBRARIES_DIR.string() + "/render_effects" );

			static const FilePath SETTING_JSON_FILE = FilePath( EXECUTABLE_DIR.string() + "/../../setting.json" );
			static const FilePath FFMPEG_EXE_FILE	= FilePath( "bin/ffmpeg.exe" );

			inline FilePath * const getDataPathPtr( const std::string & p_filename )
			{
				return new FilePath( DATA_DIR.string() + "/" + p_filename );
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

			inline void generateUniqueFileName( FilePath & p_path )
			{
				uint counter = 2;

				std::string defaultFileName = p_path.filename().string();

				while ( std::filesystem::exists( p_path ) )
				{
					p_path.replace_filename( defaultFileName + " " + std::to_string( counter ) );
					counter++;
				}
			}

			inline bool copyFile( const FilePath & p_from, FilePath & p_to, const bool p_forceCreateNew )
			{
				bool succeed = false;

				if ( std::filesystem::exists( p_to ) )
				{
					if ( p_forceCreateNew )
						generateUniqueFileName( p_to );
					else
						std::filesystem::remove( p_to );
				}

				try
				{
					std::filesystem::copy( p_from, p_to );
					succeed = true;
				}
				catch ( std::exception e )
				{
					VTX_WARNING( "Cannot copy file " + p_from.string() + " to " + p_to.string() + " : " + e.what() );
					succeed = false;
				}

				return succeed;
			}

			inline void clearDirectory( const FilePath & p_directory )
			{
				try
				{
					std::filesystem::remove_all( p_directory );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Error when clear directory " + p_directory.string() + " : " + e.what() );
				}
			}
		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
