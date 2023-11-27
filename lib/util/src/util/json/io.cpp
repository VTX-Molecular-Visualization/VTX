#include "util/json/io.hpp"
#include "util/exceptions.hpp"
#include "util/json/document.hpp"
#include "util/logger.hpp"
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>

namespace VTX::Util::JSon
{
	Document IO::open( const FilePath & p_filepath )
	{
		std::ifstream fstream;
		fstream.open( p_filepath.string() );

		if ( !fstream.is_open() )
		{
			throw IOException( "Can not read file: " + p_filepath.string() );
		}

		const std::string fileContent( std::istreambuf_iterator<char> { fstream }, {} );
		fstream.close();

		return Document::createFromString( fileContent );
	}

	void IO::write( const FilePath & p_filepath, const Document & p_json )
	{
		std::ofstream fstream;
		fstream.open( p_filepath.string() );

		if ( fstream.is_open() )
		{
			fstream << p_json.getContentAsString();
			fstream.close();
		}
		else
		{
			throw IOException( "Unable to open file: " + p_filepath.string() );
		}
	}
} // namespace VTX::Util::JSon
