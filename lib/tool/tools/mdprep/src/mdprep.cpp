#include "tools/mdprep/mdprep.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		auto module_ptr = gmx::pdb2gmxInfo::create();
		module_ptr->runAsMain(
			args.i, args.s, "ctx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
		);
		/*
		gmx::Options opt;

		gmx::CommandLineParser parser( &opt );

		parser.parse( &args.i, args.s );

		opt.finish();
		*/
		/*
		gmx::CommandLineProgramContext & context = gmx::initForCommandLine( &args.i, &args.s );
		gmx::CommandLineModuleManager	 manager( "gmx", &context );
		// registerTrajectoryAnalysisModules( &manager );
		manager.addModule( gmx::CommandLineModulePointer( new gmx::CommandLineOptionsModule(
			gmx::pdb2gmxInfo::name, gmx::pdb2gmxInfo::shortDescription, FactoryMethod( &gmx::pdb2gmxInfo::create )
		) ) );
		{
			gmx::CommandLineModuleGroup group = manager.addModuleGroup( "Generating topologies and coordinates" );
			// group.addModuleWithDescription( "editconf", "Edit the box and write subgroups" );
			//  group.addModule( "x2top" );
			// group.addModule( "solvate" );
			// group.addModule( "insert-molecules" );
			//  group.addModule( "genconf" );
			//  group.addModule( "genion" );
			//  group.addModule( "genrestr" );
			group.addModule( "pdb2gmx" );
		}

		manager.addHelpTopic( gmx::createSelectionHelpTopic() );
		int rc = manager.run( args.i, args.s );
		gmx::finalizeForCommandLine();
		*/
	}

} // namespace vtx::tool::mdprep
