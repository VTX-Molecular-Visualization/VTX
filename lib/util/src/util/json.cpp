#include "util/json.hpp"
#include "util/exceptions.hpp"

namespace VTX::Util::Json
{
	Document read( const FilePath & p_path )
	{
		const std::string content = Filesystem::readPath( p_path );

		try
		{
			return Document::parse( content );
		}
		catch ( const nlohmann::json::parse_error & p_e )
		{
			throw IOException( "Can not parse {}: {}", p_path.string(), p_e.what() );
		}
	}

	void write( const FilePath & p_path, const Document & p_document, const int p_indent )
	{ Filesystem::writeFile( p_path, p_document.dump( p_indent ) + '\n' ); }
} // namespace VTX::Util::Json
