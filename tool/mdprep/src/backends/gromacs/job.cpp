#include "tool/mdprep/backends/gromacs/job.hpp"
#include <qprocess.h>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	namespace
	{

		void fillMissingString( const QByteArray & from, std::string & to ) noexcept { to += from.toStdString(); }

		void fillMissingString( Channels & channels, std::string & err, std::string & out )
		{
			channels.stderr_ += err;
			channels.stdout_ += out;

			err.clear();
			out.clear();
		}

		void simpleProcessManagement( QProcess & p_proc, GromacsJobData & p_args ) noexcept
		{
			auto drain = [ & ]
			{
				QByteArray bufErr	= p_proc.readAllStandardError();
				QByteArray bufOut	= p_proc.readAllStandardOutput();
				auto	   channels = p_args.channelsLocker.open();
				fillMissingString( bufErr, channels->stderr_ );
				fillMissingString( bufOut, channels->stdout_ );
			};

			// waitForFinished returns true once the process has actually exited;
			// on timeout we drain accumulated output and loop again.
			while ( !p_proc.waitForFinished( 50 ) && p_proc.state() != QProcess::NotRunning )
			{
				drain();
			}

			drain(); // flush whatever was buffered after exit
		}
	} // namespace

	void submitGromacsJob( const fs::path & p_gmxExe, GromacsJobData & p_args )
	{
		auto		tmp = qgetenv( "GMXLIB" );
		QString		pgm { p_gmxExe.string().data() };
		QStringList qtArgs;
		for ( auto & arg : p_args.arguments )
		{
			qtArgs << QString( arg.c_str() );
		}

		QProcess proc;

		proc.setProgram( pgm );
		proc.setArguments( qtArgs );
		proc.start();
		proc.waitForStarted( -1 );

		simpleProcessManagement( proc, p_args );
		p_args.report.finished = true;
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
