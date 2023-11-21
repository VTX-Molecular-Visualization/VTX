#include <commandline/cmdlineparser.h>
#include <gromacs/options/options.h>

namespace
{
	void test_linking()
	{
		const char			   cmd[ 200 ] = "";
		gmx::Options		   option;
		gmx::CommandLineParser parser( option );
		parser.parse( 1, cmd );
	}
} // namespace
