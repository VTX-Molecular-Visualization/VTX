#ifndef __VTX_APP_OLD_MANAGER_WORKER_BASE_THREAD__
#define __VTX_APP_OLD_MANAGER_WORKER_BASE_THREAD__

// TODO reimplemente that without Qt and check thread which inherts from BaseThread
// #include <QThread>
#include <functional>
#include <thread>
#include <util/types.hpp>

namespace VTX::App::Old::Core::Worker
{
	class BaseThread // : public QThread
	{
		// Q_OBJECT

	  public:
		BaseThread()		  = default;
		virtual ~BaseThread() = default;

		void start()
		{
			// QThread::start();
			_run();
		}

		void run() {}
		// void run() override
		//{
		//	emit resultReady( this, _run() );
		// }

		void wait()
		{
			// QThread::wait();
		}
		void quit()
		{
			// QThread::quit();
		}

		// signals:
		// void resultReady( BaseThread *, const uint p_returnCode ) const;
		// void updateProgress( BaseThread *, const uint p_progress ) const;
		// void logInfo( const std::string p_msg ) const;
		// void logWarning( const std::string p_msg ) const;
		// void logError( const std::string p_msg ) const;
		// void logDebug( const std::string p_msg ) const;
		// void logFile( const std::string p_msg ) const;

		void emitLogInfo( const std::string p_msg ) const
		{ /*emit logInfo( p_msg );*/
		}
		void emitLogWarning( const std::string p_msg ) const
		{ /*emit logWarning( p_msg );*/
		}
		void emitLogError( const std::string p_msg ) const
		{ /*emit logError( p_msg );*/
		}
		void emitLogDebug( const std::string p_msg ) const
		{ /*emit logDebug( p_msg );*/
		}
		void emitLogFile( const std::string p_msg ) const
		{ /*emit logFile( p_msg );*/
		}

	  protected:
		virtual uint _run() = 0;
	};
} // namespace VTX::App::Old::Core::Worker
#endif
