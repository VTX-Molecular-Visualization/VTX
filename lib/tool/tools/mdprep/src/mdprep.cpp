#include <gromacs/commandline/cmdlineparser.h>

namespace
{
	void test_linking()
	{
		char				   cmd[] = { '1', '1', '1' };
		int			   i		  = 1;
		gmx::CommandLineParser parser( nullptr );
		parser.parse( &i, reinterpret_cast<char**>( &cmd ) );
	}
} // namespace
