#include "tools/mdprep/gromacs.hpp"
#include "tools/mdprep/gromacs.impl.hpp"
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
			uint64_t	   currentIterationNumber			= 0;

			while ( !p_finished )
			{
				if ( p_proc.isReadable() == false || p_proc.isWritable() == false )
				{
					std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
					continue;
				}

				qBuf = p_proc.readAllStandardError();
				fillMissingString( qBuf, p_args.stderr_ );

				qBuf = p_proc.readAllStandardOutput();
				fillMissingString( qBuf, sBuf );

				if ( isWaitingInputs( sBuf ) )
				{
					p_args.stdout_ += sBuf;
				}
			}
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
