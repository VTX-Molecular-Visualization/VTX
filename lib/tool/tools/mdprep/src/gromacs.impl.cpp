#include "tools/mdprep/gromacs.impl.hpp"
#include "tools/mdprep/gromacs.util.hpp"
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

			std::string substring = match[ match.size() - 1 ].str();

			size_t chain_pos = substring.find( '\'' ) + 1;

			std::string out;
			while ( substring.size() > chain_pos && substring.at( chain_pos + 1 ) != '\'' )
			{
				out += substring.at( chain_pos );
				chain_pos++;
			}
			return out;
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

	} // namespace
	bool is_waiting_inputs( const std::string & stdout_ ) noexcept
	{
		return !stdout_.empty() && stdout_.ends_with( generic_waiting_pattern );
	}

	bool parse_expected_kw_argument( const std::string & stdout_, interactive_id & out ) noexcept
	{
		const std::regex entire_gromacs_message { "Processing chain .+?\nType a number:" };

		std::smatch match;
		bool		last_request_matched
			= std::regex_search( stdout_, match, entire_gromacs_message, std::regex_constants::match_any );

		if ( last_request_matched == false || match.size() < 1 )
			return false;

		std::string last_gromacs_input_request_string = match[ match.size() - 1 ].str();

		{
			std::string buf = get_chain( last_gromacs_input_request_string );
			if ( !buf.empty() )
				out.chain = buf.at( 0 );
		}
		out.kw = get_keyword( last_gromacs_input_request_string );
		if ( out.kw == interactive_keyword::none )
			return false;

		out.num = get_num( last_gromacs_input_request_string );
		if ( out.num == 0xffffffff )
			return false;
		return true;
	}

	const char * get_default_value( const interactive_keyword & ) noexcept { return nullptr; }

	uint8_t parse_option_number( const std::string & stdout_, std::string_view value ) { return 0; }

} // namespace VTX::Tool::Mdprep::Gromacs
