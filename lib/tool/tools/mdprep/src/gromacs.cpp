#include "tools/mdprep/gromacs.hpp"
#include <qprocess.h>
// #include <thread>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void fill_missing_string( QByteArray & from, std::string & to ) noexcept
	{
		to += from.toStdString();
		// to += std::string( from.constData() + to.size(), from.size() - to.size() );
	}

	void submit_gromacs_command( const fs::path & p_gmx_exe, gromacs_command_args & p_args )
	{
		QString		pgm { p_gmx_exe.string().data() };
		QStringList qt_args;
		for ( auto & arg : p_args.arguments )
			qt_args << QString( arg.c_str() );

		std::string tmp;
		for ( auto & arg : p_args.arguments )
			( tmp += arg ) += " ";

		bool	   finished = false;
		QProcess   proc;
		QByteArray buf;

		proc.connect( &proc, &QProcess::finished, [ & ] { finished = true; } );
		proc.setProgram( pgm );
		proc.setArguments( qt_args );
		proc.start();
		proc.waitForStarted( 6000 );

		if ( p_args.interactive_settings.has_value() ) {}
		else
		{
			do
			{
				proc.waitForReadyRead( 10 );
				// std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
				buf = proc.readAllStandardError();
				fill_missing_string( buf, p_args.stderr_ );
				// p_args.stderr_ += buf.toStdString();

				buf = proc.readAllStandardOutput();
				fill_missing_string( buf, p_args.stdout_ );
				// p_args.stdout_ += buf.toStdString();

			} while ( !finished );
		}

		return;

		while ( finished == false )
		{
			if ( proc.isWritable() == false )
				continue;

			proc.write( "0\n", 2 );
			proc.closeWriteChannel();

			proc.waitForReadyRead( 6000 );
			buf = proc.readAllStandardError();
			p_args.stderr_ += buf.toStdString();

			buf = proc.readAllStandardOutput();
			p_args.stdout_ += buf.toStdString();
		}
	}
} // namespace VTX::Tool::Mdprep::Gromacs
