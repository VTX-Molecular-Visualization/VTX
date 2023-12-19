#include "tools/mdprep/gromacs/gromacs.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"
#include "tools/mdprep/gromacs/gromacs.args.hpp"
#include <cstring>

#ifndef SHARED_GROMACS
#include <qprocess.h>
#endif
extern "C"
{
#ifdef SHARED_GROMACS
	void submit_gromacs_command( VTX::Tool::Mdprep::Gromacs::gromacs_command_args & args )
	{
		char ** cmd = reinterpret_cast<char **>( std::malloc( sizeof( char * ) * args.arguments.size() )
		); // gromacs appears to free the array of the string, but not the strings themselves
		for ( int i = 0; i < args.arguments.size(); i++ )
		{
			std::string & arg_str = args.arguments[ i ];
			arg_str += '\0';
			char * arg_cstr = reinterpret_cast<char *>( std::malloc( sizeof( char ) * arg_str.size() ) );
			strncpy_s( arg_cstr, arg_str.size(), arg_str.c_str(), arg_str.size() );
			cmd[ i ] = arg_cstr;
		}
		gmx::ICommandLineOptionsModule::runAsMain(
			static_cast<int>( args.arguments.size() ),
			cmd,
			"vtx-pdb2gmx",
			"command vtx-gromacs",
			[ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}
#else
	void submit_gromacs_command( VTX::Tool::Mdprep::Gromacs::gromacs_command_args & args )
	{
		QString		pgm { "D:\\cnam\\tmp\\gmx.exe" };
		QStringList qt_args;
		for ( auto & arg : args.arguments )
			qt_args << QString( arg.c_str() );

		QProcess proc;

		proc.setProgram( pgm );
		proc.setArguments( qt_args );
		proc.start();

		proc.waitForFinished( 60000 );
		auto out	 = proc.readAllStandardError();
		args.stderr_ = out.toStdString();
		out			 = proc.readAllStandardOutput();
		args.stdout_ += out.toStdString();
	}
#endif
}
