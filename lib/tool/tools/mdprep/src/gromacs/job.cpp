#include "tools/mdprep/gromacs/inputs.hpp"
// Input should be first as is forward declared below
#include "tools/mdprep/gromacs/job.hpp"
#include "tools/mdprep/gromacs/pdb2gmx.impl.hpp"
#include <qprocess.h>
#include <thread>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Gromacs
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

		void interactiveProcessManagement( QProcess & p_proc, bool & p_finished, GromacsJobData & p_args ) noexcept
		{
			std::string unsentOutBuf, unsetErrBuf, // Used when the stdout is not ready to be sent
				bufErr, bufOut;
			const uint64_t MAXIMUM_WAITING_ITERATION_NUMBER = 10000;
			uint64_t	   currentIterationNumber			= 0; // Used to kill the process if too long
			size_t		   lastStdChannelSizeSum			= 0; // Used to see if gromacs wrote something

			while ( !p_finished )
			{
				size_t savedChannelsSizeSum = 0;
				{
					auto channels		 = p_args.channelsLocker.open();
					savedChannelsSizeSum = channels->stderr_.size() + channels->stdout_.size();

					if ( bufErr.empty() == false || bufOut.empty() == false )
						fillMissingString( *channels, bufErr, bufOut );
				}

				p_proc.waitForReadyRead( 10000 );

				if ( currentIterationNumber > MAXIMUM_WAITING_ITERATION_NUMBER )
				{
					p_proc.kill();
					bufErr += "\nVTX error -- gromacs was very long and might encountered unexpected difficulties. The process was killed.";
					break;
				}
				if ( p_proc.isReadable() == false || p_proc.isWritable() == false )
				{
					std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
					continue;
				}

				fillMissingString( p_proc.readAllStandardError(), unsetErrBuf );
				fillMissingString( p_proc.readAllStandardOutput(), unsentOutBuf );

				size_t currentStdChannelSizeSum = savedChannelsSizeSum + unsetErrBuf.size() + unsentOutBuf.size();
				bool   gromacsIsWaitingInputs	= p_args.interactiveSettings->isWaitingForInput( unsentOutBuf )
											  || p_args.interactiveSettings->isWaitingForInput( unsetErrBuf );

				if ( gromacsIsWaitingInputs == false && currentStdChannelSizeSum == lastStdChannelSizeSum )
				{
					currentIterationNumber++; // Nothing happened so this count as an waiting iteration
					std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
					continue;
				}
				if ( gromacsIsWaitingInputs == false )
					continue;

				// From here, gromacs wait for us to write something

				lastStdChannelSizeSum = currentStdChannelSizeSum;
				bufOut += unsentOutBuf;
				unsentOutBuf.clear();
				bufErr += unsetErrBuf;
				unsetErrBuf.clear();

				p_args.interactiveSettings->enterInput( p_proc, bufOut, bufErr );
			}
			auto channels = p_args.channelsLocker.open();
			fillMissingString( p_proc.readAllStandardError(), bufErr );
			fillMissingString( p_proc.readAllStandardOutput(), bufOut );
			fillMissingString( *channels, bufErr, bufOut );
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

		if ( p_args.interactiveSettings.has_value() )
			interactiveProcessManagement( proc, finished, p_args );
		else
			simpleProcessManagement( proc, finished, p_args );
		p_args.report.finished = true;
	}
} // namespace VTX::Tool::Mdprep::Gromacs
