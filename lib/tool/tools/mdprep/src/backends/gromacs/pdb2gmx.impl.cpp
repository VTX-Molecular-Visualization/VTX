#include "tools/mdprep/backends/gromacs/pdb2gmx.hpp"
#include <tools/mdprep/backends/gromacs/inputs.hpp>
//
#include "tools/mdprep/backends/gromacs/job.hpp"
#include "tools/mdprep/backends/gromacs/util.hpp"
#include <format>
#include <regex>
#include <util/string.hpp>
//
#include "tools/mdprep/backends/gromacs/pdb2gmx.impl.hpp"

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	namespace
	{
		const char * g_genericWaitingPattern = "\nType a number:";

		// Return current chain being parametrized
		std::string getChain( const std::string & p_stdout ) noexcept
		{
			const std::regex chainLetter { "Processing chain [0-9]+ '([A-Z]+)'" };
			std::string		 out;
			for ( auto it = std::sregex_iterator( p_stdout.begin(), p_stdout.end(), chainLetter );
				  it != std::sregex_iterator();
				  ++it )
			{
				out = it->operator[]( 1 ).str();
			}
			return out;
		}
		E_INTERACTIVE_KEYWORD getKeyword( const std::string & lastInputAskingMessage ) noexcept
		{
			if ( lastInputAskingMessage.find( "LYSINE" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::lys;
			if ( lastInputAskingMessage.find( "ARGININE" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::arg;
			if ( lastInputAskingMessage.find( "ASPARTIC ACID" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::asp;
			if ( lastInputAskingMessage.find( "GLUTAMIC ACID" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::glu;
			if ( lastInputAskingMessage.find( "GLUTAMINE" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::gln;
			if ( lastInputAskingMessage.find( "HISTIDINE" ) != std::string::npos )
				return E_INTERACTIVE_KEYWORD::his;
			return E_INTERACTIVE_KEYWORD::none;
		}
		uint32_t getNum( const std::string & lastInputAskingMessage ) noexcept
		{
			const std::regex resNumRegex( "type do you want for residue [0-9]+\r?\n" );
			const std::regex numRegex( "[0-9]+" );
			std::smatch		 match;
			bool			 matched
				= std::regex_search( lastInputAskingMessage, match, resNumRegex, std::regex_constants::match_any );

			if ( !matched || match.size() < 1 )
				return 0xffffffff;

			std::string substring = match[ match.size() - 1 ].str();
			match				  = std::smatch();
			matched				  = std::regex_search( substring, match, numRegex, std::regex_constants::match_any );

			if ( !matched || match.size() < 1 )
				return 0xffffffff;

			std::string numStr = match[ match.size() - 1 ].str();

			return std::stoul( numStr );
		}
		std::string getLastInputRequest( const std::string & p_stdout ) noexcept
		{
			const std::regex entireGromacsMessage {
				"Which .+? type do you want for residue [0-9]+\r?\n[^]+?\r?\nType a number:"
			};

			std::string lastGromacsInputRequestString {};
			for ( auto it = std::sregex_iterator( p_stdout.begin(), p_stdout.end(), entireGromacsMessage );
				  it != std::sregex_iterator();
				  ++it )
			{
				lastGromacsInputRequestString = it->str();
			}
			return lastGromacsInputRequestString;
		}

	} // namespace
	bool isWaitingInputs( const std::string_view & p_stdout ) noexcept
	{
		return !p_stdout.empty() && p_stdout.ends_with( g_genericWaitingPattern );
	}

	bool parseExpectedKwArgument( const std::string & p_stdout, Pdb2gmxInputId & out ) noexcept
	{
		{
			std::string buf = getChain( p_stdout );
			if ( !buf.empty() )
				out.chain = buf.at( 0 );
		}
		std::string lastGromacsInputRequestString = getLastInputRequest( p_stdout );
		out.kw									  = getKeyword( lastGromacsInputRequestString );
		if ( out.kw == E_INTERACTIVE_KEYWORD::none )
			return false;

		out.num = getNum( lastGromacsInputRequestString );
		if ( out.num == 0xffffffff )
			return false;
		return true;
	}

	const char * getDefaultValue( const E_INTERACTIVE_KEYWORD & p_kw ) noexcept
	{
		switch ( p_kw )
		{
		case E_INTERACTIVE_KEYWORD::none: return "1";
		case E_INTERACTIVE_KEYWORD::ss: return "1";
		case E_INTERACTIVE_KEYWORD::ter: return "0";
		case E_INTERACTIVE_KEYWORD::lys: return "1";
		case E_INTERACTIVE_KEYWORD::arg: return "1";
		case E_INTERACTIVE_KEYWORD::asp: return "1";
		case E_INTERACTIVE_KEYWORD::glu: return "1";
		case E_INTERACTIVE_KEYWORD::gln: return "1";
		case E_INTERACTIVE_KEYWORD::his: return "0";
		default: break;
		}
		return "0"; // We chose to output a default value that should works everytime
	}

	const char * g_notProtonated = "NOT PROTONATED";
	const char * g_protonated	 = "PROTONATED";

	uint8_t parseOptionNumber( const std::string & p_stdout, const std::string_view & p_value )
	{
		if ( p_value.empty() )
			return 0xffui8;
		uint8_t out = 0xffui8;
		std::from_chars( p_value.data(), p_value.data() + p_value.size(), out );
		if ( out != 0xffui8 )
			return out;

		std::string lastGromacsInputRequestString = getLastInputRequest( p_stdout );

		const std::regex gromacsOptionRegex { "[0-9]+\\. .+\n" };
		std::string		 upperInput { p_value };
		Util::String::toUpper( upperInput );

		// "protonated" match also "not protonated", so we need to fix that
		bool userInputContainsNotProtonated = ( upperInput.find( g_notProtonated ) != std::string::npos );
		bool userInputContainsProtonated	= ( upperInput.find( g_protonated ) != std::string::npos );

		for ( auto it = std::sregex_iterator(
				  lastGromacsInputRequestString.begin(), lastGromacsInputRequestString.end(), gromacsOptionRegex
			  );
			  it != std::sregex_iterator();
			  ++it )
		{
			out						= 0xffui8;
			const std::string itStr = it->str();
			std::string		  itStrUp { itStr };
			Util::String::toUpper( itStrUp );

			std::from_chars(
				itStr.data(), itStr.data() + itStr.size(), out
			); // this function should use first chars to fill the number and stop when a non-number char is found
			// Now I wonder if we should check the out value to make sure its not 0xff

			bool currentOptionIsNotProtonated = itStrUp.find( g_notProtonated ) != std::string::npos;

			const std::regex input_regex { std::format( "([^\\w]|^){}([^\\w]|$)", upperInput ) };
			std::smatch		 match; // we don't actually use it

			if ( currentOptionIsNotProtonated && userInputContainsNotProtonated )
				return out;
			if ( currentOptionIsNotProtonated && userInputContainsProtonated )
				continue; // This allow us to avoid "potonated" to match with "not protonated"
			if ( std::regex_search( itStrUp, match, input_regex ) )
				return out;
		}
		return 0xffui8;
	}

} // namespace VTX::Tool::Mdprep::backends::Gromacs
