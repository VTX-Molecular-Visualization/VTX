#include "tools/mdprep/gromacs.impl.hpp"

#include <regex>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const char * generic_waiting_pattern = "\nType a number:";
	}
	bool is_waiting_inputs( const std::string & stdout_ ) noexcept
	{
		return !stdout_.empty() && stdout_.ends_with( generic_waiting_pattern );
	}

	bool parse_expected_kw_argument( const std::string & stdout_, interactive_id & ) noexcept { 
		
		const std::regex entire_gromacs_message { "Processing chain .+?\nType a number:" };
		const std::regex chain_letter { "Processing chain [0-9]+ '([A-Z]+)'" };
		
		std::smatch match;
		bool		out = std::regex_search( stdout_, match, entire_gromacs_message, std::regex_constants::match_any );
		if ( out == false || match.size() < 1)
			return false; 

		std::string last_match = match[ match.size() - 1 ].str();


	
	
	}

	const char * get_default_value( const interactive_keyword & ) noexcept { return nullptr; }

	uint8_t parse_option_number( const std::string & stdout_, std::string_view value ) { return 0; }


} // namespace VTX::Tool::Mdprep::Gromacs