#include "tools/mdprep/gromacs.hpp"
#include "tools/mdprep/gromacs.impl.hpp"
#include <qprocess.h>
#include <thread>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void fill_missing_string( QByteArray & from, std::string & to ) noexcept { to += from.toStdString(); }

	void interactive_process_management( QProcess & p_proc, bool & p_finished, GromacsCommandArgs & p_args ) noexcept
	{
		QByteArray	   qbuf;
		std::string	   sbuf;
		const uint64_t maximum_waiting_iteration_number = 10000;
		uint64_t	   current_iteration_number			= 0;

		while ( !p_finished )
		{
			if ( p_proc.isReadable() == false || p_proc.isWritable() == false )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
				continue;
			}

			qbuf = p_proc.readAllStandardError();
			fill_missing_string( qbuf, p_args.stderr_ );

			qbuf = p_proc.readAllStandardOutput();
			fill_missing_string( qbuf, sbuf);

			if (is_waiting_inputs(sbuf))
			{
				p_args.stdout_ += sbuf;

			}
	

		}
	}
	void simple_process_management( QProcess & p_proc, bool & p_finished, GromacsCommandArgs & p_args ) noexcept
	{
		QByteArray buf;

		do
		{
			p_proc.waitForReadyRead( 10 );
			buf = p_proc.readAllStandardError();
			fill_missing_string( buf, p_args.stderr_ );

			buf = p_proc.readAllStandardOutput();
			fill_missing_string( buf, p_args.stdout_ );

		} while ( !p_finished );
	}

	void submit_gromacs_command( const fs::path & p_gmx_exe, GromacsCommandArgs & p_args )
	{
		QString		pgm { p_gmx_exe.string().data() };
		QStringList qt_args;
		for ( auto & arg : p_args.arguments )
			qt_args << QString( arg.c_str() );

		std::string tmp;
		for ( auto & arg : p_args.arguments )
			( tmp += arg ) += " ";

		bool	 finished = false;
		QProcess proc;

		proc.connect( &proc, &QProcess::finished, [ & ] { finished = true; } );
		proc.setProgram( pgm );
		proc.setArguments( qt_args );
		proc.start();
		proc.waitForStarted( -1 );

		if ( p_args.interactive_settings.has_value() )
			interactive_process_management( proc, finished, p_args );
		else
			simple_process_management( proc, finished, p_args );
	}
} // namespace VTX::Tool::Mdprep::Gromacs
