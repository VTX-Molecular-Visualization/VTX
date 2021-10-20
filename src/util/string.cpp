#include "util/string.hpp"

namespace VTX::Util::String
{
	void trim( std::string & p_str )
	{
		trimStart( p_str );
		trimEnd( p_str );
	}
	void trimStart( std::string & p_str ) { p_str.erase( 0, p_str.find_first_not_of( WHITESPACES ) ); }
	void trimEnd( std::string & p_str ) { p_str.erase( p_str.find_last_not_of( WHITESPACES ) + 1 ); }
} // namespace VTX::Util::String
