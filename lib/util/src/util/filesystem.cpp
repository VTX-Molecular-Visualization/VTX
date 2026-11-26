#include "util/filesystem.hpp"
#include "util/exceptions.hpp"
#include "util/string.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#elif defined( __APPLE__ )
#include <mach-o/dyld.h>
#elif defined( __linux__ )
#include <climits>
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
		GetModuleFileNameW( nullptr, szPath, MAX_PATH );

		return std::filesystem::path { szPath }; // to finish the folder path with (back)slash
#elif defined( __APPLE__ )
		uint size = 0;
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
		char		  szPath[ PATH_MAX ];
		const ssize_t len = readlink( "/proc/self/exe", szPath, sizeof( szPath ) - 1 );
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

	std::string readPath( const FilePath & p_filePath )
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
		if ( not p_filePath.parent_path().empty() )
		{
			std::filesystem::create_directories( p_filePath.parent_path() );
		}

		std::ofstream outputFile( p_filePath, std::ios::binary );
		if ( not outputFile.is_open() )
		{
			throw IOException( "Can not write to " + p_filePath.string() );
		}

		outputFile << p_content;
		outputFile.close();
		if ( outputFile.fail() )
		{
			throw IOException( "Can not write to " + p_filePath.string() );
		}
	}

	// Add _2, or increment last digits until name is available.
	void generateUniqueFileName( FilePath & p_filePath )
	{
		if ( not std::filesystem::exists( p_filePath ) )
		{
			return;
		}

		const std::string stem		= p_filePath.stem().string();
		const std::string extension = p_filePath.extension().string();

		size_t digitsStart = stem.size();
		while ( digitsStart > 0 && std::isdigit( static_cast<unsigned char>( stem[ digitsStart - 1 ] ) ) )
		{
			--digitsStart;
		}

		std::string_view base	 = stem;
		uint			 counter = 2;
		uint			 value	 = 0;
		if ( digitsStart > 1 && digitsStart < stem.size() && stem[ digitsStart - 1 ] == '_'
			 && std::from_chars( stem.data() + digitsStart, stem.data() + stem.size(), value ).ec == std::errc {} )
		{
			base	= std::string_view( stem ).substr( 0, digitsStart - 1 );
			counter = value + 1;
		}

		const FilePath parentPath = p_filePath.parent_path();
		do
		{
			p_filePath = parentPath / fmt::format( "{}_{}{}", base, counter++, extension );
		} while ( std::filesystem::exists( p_filePath ) );
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

	bool hasExtension( const FilePath & p_filePath, const std::initializer_list<std::string_view> p_extensions )
	{
		const std::string extension = p_filePath.extension().string();
		return std::ranges::any_of(
			p_extensions,
			[ &extension ]( const std::string_view p_extension )
			{ return String::equalsIgnoreCase( p_extension, extension ); }
		);
	}

} // namespace VTX::Util::Filesystem
