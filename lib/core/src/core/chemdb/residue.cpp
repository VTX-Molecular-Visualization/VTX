#include "core/chemdb/residue.hpp"
#include <algorithm>
#include <optional>
#include <util/enum.hpp>
#include <util/string.hpp>

namespace VTX::Core::ChemDB::Residue
{

	const SYMBOL getSymbolFromShortName( const std::string & p_residueName )
	{
		// Upcase residue name
		std::string upcasedName = Util::String::toUpper( p_residueName );

		for ( int i = 0; i < int( SYMBOL::COUNT ); i++ )
		{
			if ( SYMBOL_SHORT_STR[ i ] == upcasedName )
				return SYMBOL( i );
		}

		return SYMBOL::UNKNOWN;
	}

	const SYMBOL getSymbolFromName( const std::string & p_residueName )
	{
		std::string upcasedName = Util::String::toUpper( p_residueName );

		// Convert name
		const std::optional symbol = magic_enum::enum_cast<ChemDB::Residue::SYMBOL>( upcasedName );

		return symbol.value_or( SYMBOL::UNKNOWN );
	}
	const SYMBOL getSymbolFromLongName( const std::string & p_residueName )
	{
		std::string formattedName = std::string( p_residueName.begin(), p_residueName.end() );
		formattedName[ 0 ]		  = std::toupper( formattedName[ 0 ] );

		// Downcase residue name
		std::transform(
			++p_residueName.begin(),
			p_residueName.end(),
			++formattedName.begin(),
			[]( unsigned char c ) { return std::tolower( c ); }
		);

		for ( int i = 0; i < int( SYMBOL::COUNT ); i++ )
		{
			if ( SYMBOL_NAME[ i ] == formattedName )
				return SYMBOL( i );
		}

		return SYMBOL::UNKNOWN;
	}
	const SYMBOL getSymbolFromAnyName( const std::string & p_residueName )
	{
		const size_t residueNameSize = p_residueName.size();

		SYMBOL res = SYMBOL::UNKNOWN;

		// All symbols of 1 of 2 chars are the same in Symbol, shortName (DNA/RNA) or name ('?' for ASX and GLX)
		if ( residueNameSize <= 2 )
		{
			res = getSymbolFromShortName( p_residueName );
		}
		else if ( residueNameSize == 3 ) // Only ShortName has name with 3 chars
		{
			res = getSymbolFromName( p_residueName );
		}
		else // Only names has more than 3 char in their name
		{
			res = getSymbolFromLongName( p_residueName );
		}

		return res;
	}

	bool checkIfStandardFromName( const std::string & p_residueSymbol )
	{
		const std::string residueSymbol = Util::String::toUpper( p_residueSymbol );

		return std::find(
				   std::begin( ChemDB::Residue::SYMBOL_STR ), std::end( ChemDB::Residue::SYMBOL_STR ), residueSymbol
			   )
			   != std::end( ChemDB::Residue::SYMBOL_STR );
	}
} // namespace VTX::Core::ChemDB::Residue
