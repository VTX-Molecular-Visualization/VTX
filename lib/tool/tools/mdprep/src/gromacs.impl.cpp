#include "tools/mdprep/gromacs.impl.hpp"


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

	bool parse_expected_kw_argument( const std::string & stdout_, interactive_id & ) noexcept { return false; }

	const char * get_default_value( const interactive_keyword & ) noexcept { return nullptr; }

	uint8_t parse_option_number( const std::string & stdout_, std::string_view value ) { return 0; }


} // namespace VTX::Tool::Mdprep::Gromacs