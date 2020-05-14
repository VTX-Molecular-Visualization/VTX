#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"
#include <filesystem>

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			const std::string DATA_DIR		= "../data";
			const std::string SHADERS_DIR	= "../src/shaders";
			const std::string SNAPSHOTS_DIR = "../snapshots";
			const std::string RENDERS_DIR	= "../renders";
			const std::string PATHS_DIR		= "../paths";
			const std::string VIDEOS_DIR	= "../videos";

			inline IO::Path * const getDataPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( DATA_DIR );
				return new IO::Path( DATA_DIR + "/" + p_filename );
			}

			inline const IO::Path getShadersPath( const std::string & p_filename )
			{
				return IO::Path( SHADERS_DIR + "/" + p_filename );
			}

			inline const IO::Path getSnapshotsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( SNAPSHOTS_DIR );
				return IO::Path( SNAPSHOTS_DIR + "/" + p_filename );
			}

			inline const IO::Path getRendersPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( RENDERS_DIR );
				return IO::Path( RENDERS_DIR + "/" + p_filename );
			}

			inline const IO::Path getPathsPath( const std::string & p_filename )
			{
				std::filesystem::create_directories( PATHS_DIR );
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
