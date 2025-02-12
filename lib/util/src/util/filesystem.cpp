#include "util/filesystem.hpp"
#include "util/exceptions.hpp"
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace VTX::Util::Filesystem
{
	// Partly copied from https://stackoverflow.com/questions/50889647/best-way-to-get-exe-folder-path
	FilePath getExecutableDir()
	{
#ifdef _WIN32
		// Windows specific
		wchar_t szPath[ MAX_PATH ];
		GetModuleFileNameW( NULL, szPath, MAX_PATH );

		return std::filesystem::path { szPath }.parent_path() / ""; // to finish the folder path with (back)slash
#else
		return std::filesystem::current_path();
#endif
	}

	std::string getFileName( const FilePath & p_filePath )
	{
		std::string filenameWithExtension = p_filePath.filename().string();
		return filenameWithExtension.substr( 0, filenameWithExtension.find_last_of( '.' ) );
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

	void writeLineToFile( const FilePath & p_filePath, const std::string & p_line )
	{
		std::ofstream file( p_filePath, std::ios::app );
		if ( file.is_open() )
		{
			file << p_line << std::endl;
			file.close();
		}
		else
		{
			throw IOException( "Can not write to " + p_filePath.string() );
		}
	}

	void writeStringToFile( const FilePath & p_filePath, const std::string & p_line )
	{
		std::ofstream file( p_filePath, std::ios::app );
		if ( file.is_open() )
		{
			file << p_line;
			file.close();
		}
		else
		{
			throw IOException( "Can not write to " + p_filePath.string() );
		}
	}

} // namespace VTX::Util::Filesystem
