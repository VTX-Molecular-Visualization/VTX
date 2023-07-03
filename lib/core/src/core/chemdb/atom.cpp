#include "core/chemdb/atom.hpp"
#include <algorithm>
#include <magic_enum.hpp>
#include <optional>

namespace VTX::Core::ChemDB::Atom
{
	SYMBOL getSymbolFromString( const std::string & p_symbolStr )
	{
		std::string upperCaseSymbol = std::string( p_symbolStr.begin(), p_symbolStr.end() );

		std::transform( p_symbolStr.begin(),
						p_symbolStr.end(),
						upperCaseSymbol.begin(),
						[]( unsigned char c ) { return std::toupper( c ); } );

		const std::optional symbol = magic_enum::enum_cast<SYMBOL>( "A_" + upperCaseSymbol );
		return symbol.value_or( SYMBOL::UNKNOWN );
	}
} // namespace VTX::Core::ChemDB::Atom
