#include "util/filesystem.hpp"
#include "util/exceptions.hpp"
#include <filesystem>
#include <fstream>
#include <sago/platform_folders.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif defined( __APPLE__ )
#include <mach-o/dyld.h>
#elif defined( __linux__ )
#include <limits.h>
#include <unistd.h>
#endif

#include <iostream>

namespace VTX::Util::Filesystem
{
	FilePath getExecutableDir()
	{
		return getExecutable().parent_path()
			   / ""; // We want to finish the path with a [back]slash, for *some* reason ? context wanted
	}

	FilePath getExecutable()
	{
#ifdef _WIN32
		// Windows specific
		// Partly copied from https://stackoverflow.com/questions/50889647/best-way-to-get-exe-folder-path
		wchar_t szPath[ MAX_PATH ];
		GetModuleFileNameW( NULL, szPath, MAX_PATH );

		return std::filesystem::path { szPath }; // to finish the folder path with (back)slash
#elif defined( __APPLE__ )
		uint32_t size = 0;
		_NSGetExecutablePath( nullptr, &size );
		std::string path( size, '\0' );

		if ( _NSGetExecutablePath( path.data(), &size ) == 0 )
		{
			path.resize( std::char_traits<char>::length( path.c_str() ) );
			try
			{
				return std::filesystem::canonical( path );
			}
			catch ( const std::filesystem::filesystem_error & )
			{
				return std::filesystem::path { path };
			}
		}

		return std::filesystem::current_path();
#elif defined( __linux__ )
		// Linux specific
		char	szPath[ PATH_MAX ];
		ssize_t len = readlink( "/proc/self/exe", szPath, sizeof( szPath ) - 1 );
		if ( len != -1 )
		{
			szPath[ len ] = '\0';
			return std::filesystem::path { szPath };
		}
		else
		{
			return std::filesystem::current_path();
		}
#else
		return std::filesystem::current_path();
#endif
	}

	FilePath getSystemTmpFolder() { return std::filesystem::temp_directory_path(); }

	FilePath getConfigHome() { return FilePath( sago::getConfigHome() ); }

	FilePath getDataHome() { return FilePath( sago::getDataHome() ); }

	FilePath getStateDir() { return FilePath( sago::getStateDir() ); }

	FilePath getCacheDir() { return FilePath( sago::getCacheDir() ); }

	FilePath getDocumentsFolder() { return FilePath( sago::getDocumentsFolder() ); }

	FilePath getDesktopFolder() { return FilePath( sago::getDesktopFolder() ); }

	FilePath getPicturesFolder() { return FilePath( sago::getPicturesFolder() ); }

	FilePath getMusicFolder() { return FilePath( sago::getMusicFolder() ); }

	FilePath getVideoFolder() { return FilePath( sago::getVideoFolder() ); }

	FilePath getDownloadFolder() { return FilePath( sago::getDownloadFolder() ); }

	FilePath getSaveGamesFolder1() { return FilePath( sago::getSaveGamesFolder1() ); }

	FilePath getSaveGamesFolder2() { return FilePath( sago::getSaveGamesFolder2() ); }

	const std::string readPath( const FilePath & p_filePath )
	{
		std::ifstream inputFile( p_filePath );

		if ( inputFile.is_open() )
		{
			std::string fileContent(
				( std::istreambuf_iterator<char>( inputFile ) ), std::istreambuf_iterator<char>()
			);

			return fileContent;
		}
		else
		{
			throw IOException( "Can not read " + p_filePath.string() );
		}
	}

	void writeFile( const FilePath & p_filePath, const std::string & p_content )
	{
		std::filesystem::create_directories( p_filePath.parent_path() );
		std::ofstream outputFile( p_filePath, std::ios::binary );

		if ( outputFile.is_open() )
		{
			outputFile << p_content;
		}
		else
		{
			throw IOException( "Can not write to " + p_filePath.string() );
		}
	}

	void generateUniqueFileName( FilePath & p_filePath )
	{
		FilePath parentPath = p_filePath.parent_path();
		uint	 counter	= 2;
		while ( std::filesystem::exists( p_filePath ) )
		{
			p_filePath = parentPath
						 / p_filePath.stem().string().append(
							 "_" + std::to_string( counter ) + p_filePath.extension().string()
						 );
			counter++;
		}
	}

	void copyDirectory( const FilePath & p_filePathSrc, const FilePath & p_filePathDestination )
	{
		try
		{
			std::filesystem::remove_all( p_filePathDestination );
			std::filesystem::copy( p_filePathSrc, p_filePathDestination, std::filesystem::copy_options::recursive );
		}
		catch ( const std::filesystem::filesystem_error & p_e )
		{
			throw IOException( p_e.what() );
		}
	}

} // namespace VTX::Util::Filesystem
