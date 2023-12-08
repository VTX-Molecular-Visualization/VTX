#include "tools/mdprep/gromacs/gromacs.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"

extern "C"
{
	void submit_gromacs_command( VTX::Tool::Mdprep::gromacs_command_args & args )
	{
		char ** cmd = reinterpret_cast<char **>( std::malloc( sizeof( char * ) * args.arguments.size() )
		); // gromacs will free it ...
		for ( int i = 0; i < args.arguments.size(); i++ )
		{
			args.arguments[ i ] += '\0';
			cmd[ i ] = &args.arguments.at( i ).at( 0 );
		}
		gmx::ICommandLineOptionsModule::runAsMain(
			static_cast<size_t>( args.arguments.size() ),
			cmd,
			"vtx-pdb2gmx",
			"command vtx-gromacs",
			[ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}
}
