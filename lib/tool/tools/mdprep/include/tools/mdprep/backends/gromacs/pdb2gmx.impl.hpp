#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_IMPL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_IMPL__

/*
These functions will be used internally to interact with gromacs
*/

#include <string>
#include <string_view>

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	// Parse the input string in order to determine if gromacs is actually waiting for user input
	bool isWaitingInputs( const std::string_view & p_stdout ) noexcept;

	struct Pdb2gmxInputId;
	struct InteractiveArguments;

	// Parse the input string to figure what piece of information gromacs expect
	// Returns false if nothing could be figured out
	bool parseExpectedKwArgument( const std::string & p_stdout, Pdb2gmxInputId & ) noexcept;

	enum class E_INTERACTIVE_KEYWORD;

	// Returns the default string value to send to gromacs
	//  When the user's script doesn't mention a kw requested by gromacs, we will fall back on these values
	const char * getDefaultValue( const E_INTERACTIVE_KEYWORD & ) noexcept;

	// Parse the first argument to figure what is the option number requested by gromacs to refer to the option
	// specified in the second argument.
	// Returns 0xff if the option number couldn't be figured
	uint8_t parseOptionNumber( const std::string & p_stdout, const std::string_view & p_value );

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
