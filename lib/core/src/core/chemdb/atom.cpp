#include "core/chemdb/atom.hpp"
#include <algorithm>
#include <optional>
#include <util/enum.hpp>

namespace VTX::Core::ChemDB::Atom
{
	SYMBOL getSymbolFromString( const std::string & p_symbolStr )
	{
		std::string formattedSymbol = std::string( "\0", p_symbolStr.size() + 2 );

		formattedSymbol[ 0 ] = 'A';
		formattedSymbol[ 1 ] = '_';

		for ( size_t i = 0; i < p_symbolStr.size(); i++ )
		{
			formattedSymbol[ i + 2 ] = std::toupper( p_symbolStr[ i ] );
		}

		const std::optional symbol = magic_enum::enum_cast<SYMBOL>( formattedSymbol );
		return symbol.value_or( SYMBOL::UNKNOWN );
	}
} // namespace VTX::Core::ChemDB::Atom
