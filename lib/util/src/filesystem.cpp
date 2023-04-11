#include "filesystem.hpp"
#include "exceptions.hpp"
#include <fstream>

namespace VTX::Util::Filesystem
{

	const std::string readPath( const FilePath & p_filePath )
	{
		std::ifstream inputFile( p_filePath );

		if ( inputFile.is_open() )
		{
			std::string fileContent( ( std::istreambuf_iterator<char>( inputFile ) ),
									 std::istreambuf_iterator<char>() );

			return fileContent;
		}
		else
		{
			throw IOException( "Can not read " + p_filePath.string() );
		}
	}

	void generateUniqueFileName( FilePath & p_filePath )
	{
		FilePath parentPath = p_filePath.parent_path();
		uint	 counter	= 2;
		while ( std::filesystem::exists( p_filePath ) )
		{
			p_filePath
				= parentPath
				  / p_filePath.stem().append( "_" + std::to_string( counter ) + "." + p_filePath.extension().string() );
			counter++;
		}
	}

	void removeAll( const FilePath & p_filePath )
	{
		std::filesystem::remove_all( p_filePath.has_filename() ? p_filePath.parent_path() : p_filePath );
	}

} // namespace VTX::Util::Filesystem
