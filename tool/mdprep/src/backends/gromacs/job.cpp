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

		void simpleProcessManagement( QProcess & p_proc, bool & p_finished, GromacsJobData & p_args ) noexcept
		{
			QByteArray bufErr, bufOut;

			do
			{
				p_proc.waitForReadyRead( 10 );
				bufErr = p_proc.readAllStandardError();
				bufOut = p_proc.readAllStandardOutput();

				auto channels = p_args.channelsLocker.open();
				fillMissingString( bufErr, channels->stderr_ );
				fillMissingString( bufOut, channels->stdout_ );

			} while ( !p_finished );
		}
	} // namespace

	void submitGromacsJob( const fs::path & p_gmxExe, GromacsJobData & p_args )
	{
		auto		tmp = qgetenv( "GMXLIB" );
		QString		pgm { p_gmxExe.string().data() };
		QStringList qtArgs;
		for ( auto & arg : p_args.arguments )
			qtArgs << QString( arg.c_str() );

		bool	 finished = false;
		QProcess proc;

		proc.connect( &proc, &QProcess::finished, [ & ] { finished = true; } );
		proc.setProgram( pgm );
		proc.setArguments( qtArgs );
		proc.start();
		proc.waitForStarted( -1 );

		simpleProcessManagement( proc, finished, p_args );
		p_args.report.finished = true;
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
