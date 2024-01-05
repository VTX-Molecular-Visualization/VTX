#include "tools/mdprep/gromacs.impl.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include <format>
#include <regex>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const char * generic_waiting_pattern = "\nType a number:";

		std::string get_chain( const std::string & last_input_asking_message ) noexcept
		{
			const std::regex chain_letter { "Processing chain [0-9]+ '([A-Z]+)'" };
			std::smatch		 match;
			bool			 matched
				= std::regex_search( last_input_asking_message, match, chain_letter, std::regex_constants::match_any );
			if ( !matched || match.size() < 1 )
				return "";

			return match[ match.size() - 1 ].str();
		}
		interactive_keyword get_keyword( const std::string & last_input_asking_message ) noexcept
		{
			if ( last_input_asking_message.find( "LYSINE" ) != std::string::npos )
				return interactive_keyword::lys;
			if ( last_input_asking_message.find( "ARGININE" ) != std::string::npos )
				return interactive_keyword::arg;
			if ( last_input_asking_message.find( "ASPARTIC ACID" ) != std::string::npos )
				return interactive_keyword::asp;
			if ( last_input_asking_message.find( "GLUTAMIC ACID" ) != std::string::npos )
				return interactive_keyword::glu;
			if ( last_input_asking_message.find( "GLUTAMINE" ) != std::string::npos )
				return interactive_keyword::gln;
			if ( last_input_asking_message.find( "HISTIDINE" ) != std::string::npos )
				return interactive_keyword::his;
			return interactive_keyword::none;
		}
		uint32_t get_num( const std::string & last_input_asking_message ) noexcept
		{
			const std::regex res_num_regex( "type do you want for residue [0-9]+\n" );
			const std::regex num_regex( "[0-9]+" );
			std::smatch		 match;
			bool			 matched
				= std::regex_search( last_input_asking_message, match, res_num_regex, std::regex_constants::match_any );

			if ( !matched || match.size() < 1 )
				return 0xffffffff;

			std::string substring = match[ match.size() - 1 ].str();
			match				  = std::smatch();
			matched				  = std::regex_search( substring, match, num_regex, std::regex_constants::match_any );

			if ( !matched || match.size() < 1 )
				return 0xffffffff;

			std::string num_str = match[ match.size() - 1 ].str();

			return std::stoul( num_str );
		}
		std::string get_last_input_request( const std::string & stdout_ ) noexcept
		{
			const std::regex entire_gromacs_message {
				"Which .+? type do you want for residue [0-9]+\n[^]+?\nType a number:"
			};

			std::string last_gromacs_input_request_string {};
			for ( auto it = std::sregex_iterator( stdout_.begin(), stdout_.end(), entire_gromacs_message );
				  it != std::sregex_iterator();
				  ++it )
			{
				last_gromacs_input_request_string = it->str();
			}
			return last_gromacs_input_request_string;
		}

	} // namespace
	bool is_waiting_inputs( const std::string & stdout_ ) noexcept
	{
		return !stdout_.empty() && stdout_.ends_with( generic_waiting_pattern );
	}

	bool parse_expected_kw_argument( const std::string & stdout_, interactive_id & out ) noexcept
	{
		{
			std::string buf = get_chain( stdout_ );
			if ( !buf.empty() )
				out.chain = buf.at( 0 );
		}
		std::string last_gromacs_input_request_string = get_last_input_request( stdout_ );
		out.kw										  = get_keyword( last_gromacs_input_request_string );
		if ( out.kw == interactive_keyword::none )
			return false;

		out.num = get_num( last_gromacs_input_request_string );
		if ( out.num == 0xffffffff )
			return false;
		return true;
	}

	const char * get_default_value( const interactive_keyword & kw ) noexcept
	{
		switch ( kw )
		{
		case interactive_keyword::none: return "1";
		case interactive_keyword::ss: return "1";
		case interactive_keyword::ter: return "0";
		case interactive_keyword::lys: return "1";
		case interactive_keyword::arg: return "1";
		case interactive_keyword::asp: return "1";
		case interactive_keyword::glu: return "1";
		case interactive_keyword::gln: return "1";
		case interactive_keyword::his: return "0";
		default: break;
		}
		return "0"; // We chose to output a default value that should works everytime
	}

	const char * g_not_protonated = "NOT PROTONATED";
	const char * g_protonated	  = "PROTONATED";

	uint8_t parse_option_number( const std::string & stdout_, const std::string_view & value )
	{
		if ( value.empty() )
			return 0xffui8;
		uint8_t out = 0xffui8;
		std::from_chars( value.data(), value.data() + value.size(), out );
		if ( out != 0xffui8 )
			return out;

		std::string last_gromacs_input_request_string = get_last_input_request( stdout_ );

		const std::regex gromacs_option_regex { "[0-9]+\\. .+\n" };
		std::string		 upper_input { value };
		std::transform(
			upper_input.begin(), upper_input.end(), upper_input.begin(), []( char & c ) { return std::toupper( c ); }
		);

		// "protonated" match also "not protonated", so we need to fix that
		bool user_input_contains_not_protonated = ( upper_input.find( g_not_protonated ) != std::string::npos );
		bool user_input_contains_protonated		= ( upper_input.find( g_protonated ) != std::string::npos );

		for ( auto it = std::sregex_iterator(
				  last_gromacs_input_request_string.begin(),
				  last_gromacs_input_request_string.end(),
				  gromacs_option_regex
			  );
			  it != std::sregex_iterator();
			  ++it )
		{
			out						 = 0xffui8;
			const std::string it_str = it->str();
			std::string		  it_str_up { it_str };
			std::transform(
				it_str_up.begin(), it_str_up.end(), it_str_up.begin(), []( char & c ) { return std::toupper( c ); }
			);
			bool current_option_is_not_protonated	= it_str_up.find( g_not_protonated ) != std::string::npos;
			bool current_option_contains_protonated = it_str_up.find( g_protonated ) != std::string::npos;
			std::from_chars(
				it_str.data(), it_str.data() + it_str.size(), out
			); // should use first chars to fill the number and stop when a non-number char is found

			const std::regex input_regex { std::format( "([^\\w]|^){}([^\\w]|$)", upper_input ) };
			std::smatch		 match; // we don't actually use it

			if ( current_option_is_not_protonated && user_input_contains_not_protonated )
				return out;
			if ( current_option_is_not_protonated && user_input_contains_protonated )
				continue; // This allow us to avoid "potonated" to match with "not protonated"
			if ( std::regex_search( it_str_up, match, input_regex ) )
				return out;
		}
		return 0xffui8;
	}

} // namespace VTX::Tool::Mdprep::Gromacs
