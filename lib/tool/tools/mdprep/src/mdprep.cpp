#include "tools/mdprep/mdprep.hpp"
#include "gromacs/commandline/cmdlineparser.h"
#include "gromacs/options/options.h"

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		gmx::Options opt;

		gmx::CommandLineParser parser( &opt );

		parser.parse( &args.i, args.s );
		opt.finish();
	}

} // namespace vtx::tool::mdprep
