#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "../exception.hpp"
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
			static const FilePath DATA_DIR = "../data";

			inline const FilePath getCurrentDir()
			{
				char * buff;
				buff = GetCurrentDir( 0, NULL );
				FilePath currentWorkingDir( buff );
				return currentWorkingDir;
			}

			inline const FilePath getExecutableFile()
			{
#ifdef _WIN32
				wchar_t path[ MAX_PATH ] = { 0 };
				GetModuleFileNameW( NULL, path, MAX_PATH );
				FilePath exe( path );
				return exe;
#else
				char	result[ PATH_MAX ];
				ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
				return std::string( result, ( count > 0 ) ? count : 0 );
#endif
			}

			static const FilePath EXECUTABLE_FILE = getExecutableFile();
			static const FilePath EXECUTABLE_DIR  = EXECUTABLE_FILE.parent_path();
			static const FilePath SHADERS_DIR	  = FilePath( EXECUTABLE_DIR.string() + "/../../shaders" );
			static const FilePath SNAPSHOTS_DIR	  = FilePath( EXECUTABLE_DIR.string() + "/../../snapshots" );
			static const FilePath RENDERS_DIR	  = FilePath( EXECUTABLE_DIR.string() + "/../../renders" );
			static const FilePath PATHS_DIR		  = FilePath( EXECUTABLE_DIR.string() + "/../../paths" );
			static const FilePath VIDEOS_DIR	  = FilePath( EXECUTABLE_DIR.string() + "/../../videos" );
			static const FilePath LOGS_DIR		  = FilePath( EXECUTABLE_DIR.string() + "/../../logs" );
			static const FilePath LIBS_DIR		  = FilePath( EXECUTABLE_DIR.string() + "/.." );

			static const std::string IMGUI_INI_FILE
				= FilePath( EXECUTABLE_DIR.string() + "/../../imgui.ini" ).string(); // TOFIX
			static const FilePath SETTING_JSON_FILE = FilePath( EXECUTABLE_DIR.string() + "/../../setting.json" );
			static const FilePath FFMPEG_EXE_FILE	= FilePath( LIBS_DIR.string() + "/ffmpeg.exe" );

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
		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
