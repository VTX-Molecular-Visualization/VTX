#include "tool/mdprep/backends/gromacs/job.hpp"
#include <qprocess.h>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	namespace
	{

		void fillMissingString( const QByteArray & from, std::string & to ) noexcept { to += from.toStdString(); }

		void simpleProcessManagement( QProcess & p_proc, GromacsJobData & p_args )
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
			while ( p_proc.waitForFinished( 50 ) == false && p_proc.state() != QProcess::ProcessState::NotRunning )
			{
				drain();
			}
			drain(); // flush whatever was buffered after exit
		}
	} // namespace

	void submitGromacsJob( const FilePath & p_gmxExe, GromacsJobData & p_args )
	{
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
		proc.closeWriteChannel();
		if ( proc.waitForStarted( -1 ) == false )
		{
			p_args.report.errorOccured = true;
			p_args.report.errors.emplace_back( "Process could not be started" );
			return;
		}
		simpleProcessManagement( proc, p_args );
		p_args.report.finished = true;
		if ( proc.exitStatus() == QProcess::ExitStatus::CrashExit || proc.exitCode() != 0 )
		{
			p_args.report.errorOccured = true;
			p_args.report.errors.emplace_back( fmt::format( "Process exited with code {}", proc.exitCode() ) );
		}
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
