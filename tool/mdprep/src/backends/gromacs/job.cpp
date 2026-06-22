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

	void submitGromacsJob( const FilePath & p_gmxExe, const FilePath & p_workingDirectory, GromacsJobData & p_args )
	{
		QString		pgm { p_gmxExe.string().data() };
		QStringList qtArgs;
		std::string command = p_gmxExe.string();
		for ( auto & arg : p_args.arguments )
		{
			qtArgs << QString( arg.c_str() );
			command += fmt::format( " \"{}\"", arg );
		}
		VTX_INFO( "[MDPREP] Running Gromacs command: {}", command );

		QProcess proc;

		proc.setProgram( pgm );
		proc.setArguments( qtArgs );
		const FilePath absoluteWorkingDirectory = fs::absolute( p_workingDirectory );
		proc.setWorkingDirectory( QString::fromStdString( absoluteWorkingDirectory.string() ) );
		VTX_INFO( "[MDPREP] Gromacs working directory: {}", absoluteWorkingDirectory.string() );
		proc.start();
		proc.closeWriteChannel();
		if ( proc.waitForStarted( -1 ) == false )
		{
			p_args.report.errorOccured = true;
			p_args.report.errors.emplace_back(
				fmt::format( "Process could not be started: {}", proc.errorString().toStdString() )
			);
			VTX_ERROR(
				"[MDPREP] Unable to start Gromacs executable <{}> (exists: {}): {}",
				p_gmxExe.string(),
				fs::exists( p_gmxExe ),
				proc.errorString().toStdString()
			);
			return;
		}
		simpleProcessManagement( proc, p_args );
		p_args.report.finished = true;
		VTX_DEBUG(
			"[MDPREP] Gromacs process finished with exit code {} and status {}.",
			proc.exitCode(),
			static_cast<int>( proc.exitStatus() )
		);
		if ( proc.exitStatus() == QProcess::ExitStatus::CrashExit || proc.exitCode() != 0 )
		{
			p_args.report.errorOccured = true;
			p_args.report.errors.emplace_back( fmt::format( "Process exited with code {}", proc.exitCode() ) );
			VTX_ERROR( "[MDPREP] Gromacs process failed with exit code {}.", proc.exitCode() );
		}
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
