#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/gromacs/gromacs.impl.hpp"
#include <qprocess.h>
#include <thread>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{

		void fillMissingString( QByteArray & from, std::string & to ) noexcept { to += from.toStdString(); }

		void interactiveProcessManagement( QProcess & p_proc, bool & p_finished, GromacsCommandArgs & p_args ) noexcept
		{
			QByteArray	   qBuf;
			std::string	   sBuf;
			const uint64_t MAXIMUM_WAITING_ITERATION_NUMBER = 10000;
			uint64_t	   currentIterationNumber			= 0; // Used to kill the process if too long
			size_t		   lastStdChannelSizeSum			= 0; // Used to see if gromacs wrote something

			while ( !p_finished )
			{
				p_proc.waitForReadyRead();

				if ( currentIterationNumber > MAXIMUM_WAITING_ITERATION_NUMBER )
				{
					p_proc.kill();
					p_args.stderr_ += "\nVTX error -- gromacs was very long and might encountered unexpected difficulties. The process was killed.";
					break;
				}
				if ( p_proc.isReadable() == false || p_proc.isWritable() == false )
				{
					std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
					continue;
				}

				qBuf = p_proc.readAllStandardError();
				fillMissingString( qBuf, p_args.stderr_ );

				qBuf = p_proc.readAllStandardOutput();
				fillMissingString( qBuf, sBuf );

				size_t currentStdChannelSizeSum = p_args.stderr_.size() + sBuf.size() + p_args.stdout_.size();
				bool   gromacsIsWaitingInputs	= isWaitingInputs( sBuf );

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
				p_args.stdout_ += sBuf;
				sBuf.clear();

				InteractiveId expectedId;
				bool noProblemFoundWhileParsingGromacsOutput = parseExpectedKwArgument( p_args.stdout_, expectedId );
				if ( noProblemFoundWhileParsingGromacsOutput == false )
				{
					p_proc.kill();
					p_args.stderr_ += "\nVTX error -- VTX didn't recognized gromacs expected value.";
					break;
				}

				const char * value = nullptr;
				if ( p_args.interactiveSettings->kwValue.contains( expectedId ) )
					value = p_args.interactiveSettings->kwValue.at( expectedId ).data();
				else
					value = getDefaultValue( expectedId.kw );
				std::string bytesForGromacs { value };
				bytesForGromacs += '\n';
				p_proc.write( bytesForGromacs.data() );
				p_args.stdout_ += bytesForGromacs;
				p_proc.waitForBytesWritten();
			}
			fillMissingString( qBuf, p_args.stderr_ );
			p_args.stdout_ += sBuf;
		}
		void simpleProcessManagement( QProcess & p_proc, bool & p_finished, GromacsCommandArgs & p_args ) noexcept
		{
			QByteArray buf;

			do
			{
				p_proc.waitForReadyRead( 10 );
				buf = p_proc.readAllStandardError();
				fillMissingString( buf, p_args.stderr_ );

				buf = p_proc.readAllStandardOutput();
				fillMissingString( buf, p_args.stdout_ );

			} while ( !p_finished );
		}
	} // namespace

	void submitGromacsCommand( const fs::path & p_gmxExe, GromacsCommandArgs & p_args )
	{
		QString		pgm { p_gmxExe.string().data() };
		QStringList qtArgs;
		for ( auto & arg : p_args.arguments )
			qtArgs << QString( arg.c_str() );

		std::string tmp;
		for ( auto & arg : p_args.arguments )
			( tmp += arg ) += " ";

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
	}
} // namespace VTX::Tool::Mdprep::Gromacs
