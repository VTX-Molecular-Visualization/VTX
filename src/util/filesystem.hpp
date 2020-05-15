#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"
#include <filesystem>
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
			static const IO::Path DATA_DIR = "../data";

			inline const IO::Path getCurrentDir()
			{
				char buff[ FILENAME_MAX ];
				GetCurrentDir( buff, FILENAME_MAX );
				IO::Path currentWorkingDir( buff );
				return currentWorkingDir;
			}

			inline const IO::Path getExecutableFile()
			{
#ifdef _WIN32
				wchar_t path[ MAX_PATH ] = { 0 };
				GetModuleFileNameW( NULL, path, MAX_PATH );
				IO::Path exe( std::filesystem::path( path ).string() );
				return exe;
#else
				char	result[ PATH_MAX ];
				ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
				return std::string( result, ( count > 0 ) ? count : 0 );
#endif
			}

			static const IO::Path EXECUTABLE_FILE = getExecutableFile();
			static const IO::Path EXECUTABLE_DIR  = EXECUTABLE_FILE.getDirectory();
			static const IO::Path SHADERS_DIR	  = EXECUTABLE_DIR + "/../../shaders";
			static const IO::Path SNAPSHOTS_DIR	  = EXECUTABLE_DIR + "/../../snapshots";
			static const IO::Path RENDERS_DIR	  = EXECUTABLE_DIR + "/../../renders";
			static const IO::Path PATHS_DIR		  = EXECUTABLE_DIR + "/../../paths";
			static const IO::Path VIDEOS_DIR	  = EXECUTABLE_DIR + "/../../videos";
			static const IO::Path IMGUI_INI_FILE  = EXECUTABLE_DIR + "/../../imgui.ini";

			inline IO::Path * const getDataPathPtr( const std::string & p_filename )
			{
				return new IO::Path( DATA_DIR + "/" + p_filename );
			}

			inline const IO::Path getShadersPath( const std::string & p_filename )
			{
				return IO::Path( SHADERS_DIR + "/" + p_filename );
			}

			inline const IO::Path getSnapshotsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( SNAPSHOTS_DIR.str() );
				return IO::Path( SNAPSHOTS_DIR + "/" + p_filename );
			}

			inline const IO::Path getRendersPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( RENDERS_DIR.str() );
				return IO::Path( RENDERS_DIR + "/" + p_filename );
			}

			inline const IO::Path getPathsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( PATHS_DIR.str() );
				return IO::Path( PATHS_DIR + "/" + p_filename );
			}

			inline const IO::Path getVideosPath( const std::string & p_batchName, const std::string & p_fileName )
			{
				std::string dir = VIDEOS_DIR + "/" + p_batchName;
				std::filesystem::create_directories( dir );
				return IO::Path( dir + "/" + p_fileName );
			}

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
