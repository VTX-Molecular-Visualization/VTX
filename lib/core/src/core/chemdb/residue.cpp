#include "core/chemdb/residue.hpp"
#include <algorithm>
#include <magic_enum.hpp>
#include <optional>

namespace VTX::Core::ChemDB::Residue
{
	const Util::Color::Rgba & getResidueColor( const SYMBOL p_symbol, const bool isStd )
	{
		return isStd ? SYMBOL_COLOR[ int( p_symbol ) ] : SYMBOL_COLOR[ int( ChemDB::Residue::SYMBOL::UNKNOWN ) ];
	}

	const SYMBOL getSymbolFromName( const std::string & p_residueName )
	{
		std::string upcasedName = std::string( p_residueName.begin(), p_residueName.end() );

		// Upcase residue name
		std::transform( p_residueName.begin(),
						p_residueName.end(),
						upcasedName.begin(),
						[]( unsigned char c ) { return std::toupper( c ); } );

		// Convert name
		const std::optional symbol = magic_enum::enum_cast<ChemDB::Residue::SYMBOL>( upcasedName );

		return symbol.value_or( SYMBOL::UNKNOWN );
	}

	bool checkIfStandardFromName( const std::string & p_residueSymbol )
	{
		std::string residueSymbol = p_residueSymbol;
		std::transform( residueSymbol.begin(),
						residueSymbol.end(),
						residueSymbol.begin(),
						[]( unsigned char c ) { return std::toupper( c ); } );

		return std::find(
				   std::begin( ChemDB::Residue::SYMBOL_STR ), std::end( ChemDB::Residue::SYMBOL_STR ), residueSymbol )
			   != std::end( ChemDB::Residue::SYMBOL_STR );
	}
} // namespace VTX::Core::ChemDB::Residue
