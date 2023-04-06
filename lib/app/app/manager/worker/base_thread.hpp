#ifndef __VTX_BASE_THREAD__
#define __VTX_BASE_THREAD__

// TODO remove Qt from WorkerManager
// #include <QThread>
#include <functional>
#include <thread>

namespace VTX
{
	namespace Worker
	{
		class BaseThread // : public QThread
		{
		  public:
			BaseThread()		  = default;
			virtual ~BaseThread() = default;

			void quit() {}
			void wait() {}
			void run() { _run(); }

			// void run() override { emit resultReady( this, _run() ); }

			//		  signals:
			void resultReady( BaseThread *, const uint p_returnCode ) const;
			void updateProgress( BaseThread *, const uint p_progress ) const;
			void logInfo( const std::string p_msg ) const;
			void logWarning( const std::string p_msg ) const;
			void logError( const std::string p_msg ) const;
			void logDebug( const std::string p_msg ) const;
			void logFile( const std::string p_msg ) const;

		  protected:
			virtual uint _run() = 0;
		};
	} // namespace Worker
} // namespace VTX
#endif
