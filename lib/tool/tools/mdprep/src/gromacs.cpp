#include "tools/mdprep/gromacs.hpp"
#include <qprocess.h>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void submit_gromacs_command( gromacs_command_args & args )
	{
		// QString pgm { "D:\\programing\\projects\\ConsoleApplication2\\x64\\Debug\\ConsoleApplication2.exe" };
		QString		pgm { "D:\\cnam\\tmp\\gmx.exe" };
		QStringList qt_args;
		for ( auto & arg : args.arguments )
			qt_args << QString( arg.c_str() );

		std::string tmp;
		for ( auto & arg : args.arguments )
			( tmp += arg ) += " ";

		bool	   finished = false;
		QProcess   proc;
		QByteArray buf;

		proc.connect( &proc, &QProcess::finished, [ & ] { finished = true; } );
		proc.setProgram( pgm );
		proc.setArguments( qt_args );
		proc.start();

		proc.waitForStarted( 6000 );
		proc.waitForReadyRead( 6000 );

		buf = proc.readAllStandardError();
		args.stderr_ += buf.toStdString();

		buf = proc.readAllStandardOutput();
		args.stdout_ += buf.toStdString();
		while ( finished == false )
		{
			if ( proc.isWritable() == false )
				continue;

			proc.write( "0\n", 2 );
			proc.closeWriteChannel();

			proc.waitForReadyRead( 6000 );
			buf = proc.readAllStandardError();
			args.stderr_ += buf.toStdString();

			buf = proc.readAllStandardOutput();
			args.stdout_ += buf.toStdString();
		}

		proc.waitForFinished( 6000 );
	}
} // namespace VTX::Tool::Mdprep::Gromacs
