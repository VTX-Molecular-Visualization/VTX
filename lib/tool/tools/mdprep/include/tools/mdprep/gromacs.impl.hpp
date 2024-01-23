#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_IMPL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_IMPL__

/*
These functions will be used internally to interact with gromacs
*/

#include <string>
#include <string_view>

namespace VTX::Tool::Mdprep::Gromacs
{
	// Parse the input string in order to determine if gromacs is actually waiting for user input
	bool isWaitingInputs( const std::string & p_stdout ) noexcept;

	struct InteractiveId;
	struct InteractiveArguments;

	// Parse the input string to figure what piece of information gromacs expect
	// Returns false if nothing could be figured out
	bool parse_expected_kw_argument( const std::string & p_stdout, InteractiveId & ) noexcept;

	enum class interactive_keyword;

	// Returns the default string value to send to gromacs
	//  When the user's script doesn't mention a kw requested by gromacs, we will fall back on these values
	const char * get_default_value( const interactive_keyword & ) noexcept;

	// Parse the first argument to figure what is the option number requested by gromacs to refer to the option
	// specified in the second argument.
	// Returns 0xff if the option number couldn't be figured
	uint8_t parse_option_number( const std::string & p_stdout, const std::string_view & p_value );

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
