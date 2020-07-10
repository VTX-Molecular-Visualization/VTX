#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <fstream>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define GetCurrentDir _getcwd
#else
#include <limits.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			// Test only.
			static const Path DATA_DIR = "../data";

			inline const Path getCurrentDir()
			{
				char buff[ FILENAME_MAX ];
				GetCurrentDir( buff, FILENAME_MAX );
				Path currentWorkingDir( buff );
				return currentWorkingDir;
			}

			inline const Path getExecutableFile()
			{
#ifdef _WIN32
				wchar_t path[ MAX_PATH ] = { 0 };
				GetModuleFileNameW( NULL, path, MAX_PATH );
				Path exe( path );
				return exe;
#else
				char	result[ PATH_MAX ];
				ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
				return std::string( result, ( count > 0 ) ? count : 0 );
#endif
			}

			static const Path		 EXECUTABLE_FILE   = getExecutableFile();
			static const Path		 EXECUTABLE_DIR	   = EXECUTABLE_FILE.parent_path();
			static const Path		 SHADERS_DIR	   = Path( EXECUTABLE_DIR.string() + "/../../shaders" );
			static const Path		 SNAPSHOTS_DIR	   = Path( EXECUTABLE_DIR.string() + "/../../snapshots" );
			static const Path		 RENDERS_DIR	   = Path( EXECUTABLE_DIR.string() + "/../../renders" );
			static const Path		 PATHS_DIR		   = Path( EXECUTABLE_DIR.string() + "/../../paths" );
			static const Path		 VIDEOS_DIR		   = Path( EXECUTABLE_DIR.string() + "/../../videos" );
			static const Path		 LIBS_DIR		   = Path( EXECUTABLE_DIR.string() + "/.." );

			static const std::string IMGUI_INI_FILE	   = Path( EXECUTABLE_DIR.string() + "/../../imgui.ini" ).string(); // TOFIX
			static const Path		 SETTING_JSON_FILE = Path( EXECUTABLE_DIR.string() + "/../../setting.json" );
			static const Path		 FFMPEG_EXE_FILE   = Path( LIBS_DIR.string() + "/ffmpeg.exe" );

			inline Path * const getDataPathPtr( const std::string & p_filename )
			{
				return new Path( DATA_DIR.string() + "/" + p_filename );
			}

			inline const Path getShadersPath( const std::string & p_filename )
			{
				return Path( SHADERS_DIR ) /= p_filename;
			}

			inline const Path getSnapshotsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( SNAPSHOTS_DIR );
				return Path( SNAPSHOTS_DIR ) /= p_filename;
			}

			inline const Path getRendersPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( RENDERS_DIR );
				return Path( RENDERS_DIR ) /= p_filename;
			}

			inline const Path getPathsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( PATHS_DIR );
				return Path( PATHS_DIR ) /= p_filename;
			}

			inline const Path getVideosPath( const std::string & p_batchName, const std::string & p_fileName )
			{
				Path dir = Path( VIDEOS_DIR ) /= p_batchName;
				std::filesystem::create_directories( dir );
				return Path( dir ) /= p_fileName;
			}

			inline const std::string readPath( const Path & p_path )
			{
				std::ifstream file;
				file.open( p_path, std::ios::in );

				if ( !file.is_open() )
				{
					throw Exception::VTXException( "Cannot open file " + p_path.string() );
				}

				const uintmax_t size = std::filesystem::file_size( p_path );
				std::string		result( size, '\0' );
				file.read( result.data(), size );
				file.close();

				return result;
			}
		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
