#include "util/filesystem.hpp"
#include "util/exceptions.hpp"
#include <filesystem>
#include <fstream>
#include <sago/platform_folders.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

namespace VTX::Util::Filesystem
{
	// Partly copied from https://stackoverflow.com/questions/50889647/best-way-to-get-exe-folder-path
	FilePath getExecutableDir()
	{
		/*
		std::cout << "Config: " << sago::getConfigHome() << "\n";
		std::cout << "Data: " << sago::getDataHome() << "\n";
		std::cout << "State: " << sago::getStateDir() << "\n";
		std::cout << "Cache: " << sago::getCacheDir() << "\n";
		std::cout << "Documents: " << sago::getDocumentsFolder() << "\n";
		std::cout << "Desktop: " << sago::getDesktopFolder() << "\n";
		std::cout << "Pictures: " << sago::getPicturesFolder() << "\n";
		std::cout << "Music: " << sago::getMusicFolder() << "\n";
		std::cout << "Video: " << sago::getVideoFolder() << "\n";
		std::cout << "Download: " << sago::getDownloadFolder() << "\n";
		std::cout << "Save Games 1: " << sago::getSaveGamesFolder1() << "\n";
		std::cout << "Save Games 2: " << sago::getSaveGamesFolder2() << "\n";
		*/

#ifdef _WIN32
		// Windows specific
		wchar_t szPath[ MAX_PATH ];
		GetModuleFileNameW( NULL, szPath, MAX_PATH );

		return std::filesystem::path { szPath }.parent_path() / ""; // to finish the folder path with (back)slash
#else
		return std::filesystem::current_path();
#endif
	}

	FilePath getUserDataDir()
	{
		// TODO.
		return getExecutableDir();
	}

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
		std::ofstream outputFile( p_filePath );

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

	void removeAll( const FilePath & p_filePath )
	{
		try
		{
			std::filesystem::remove_all( p_filePath.has_filename() ? p_filePath.parent_path() : p_filePath );
		}
		catch ( const std::filesystem::filesystem_error & p_e )
		{
			throw IOException( p_e.what() );
		}
	}

	void copyDirectory( const FilePath & p_filePathSrc, const FilePath & p_filePathDestination )
	{
		try
		{
			removeAll( p_filePathDestination );
			std::filesystem::copy( p_filePathSrc, p_filePathDestination, std::filesystem::copy_options::recursive );
		}
		catch ( const std::filesystem::filesystem_error & p_e )
		{
			throw IOException( p_e.what() );
		}
	}

} // namespace VTX::Util::Filesystem
