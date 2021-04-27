#ifndef __VTX_BASE_WORKER__
#define __VTX_BASE_WORKER__

#ifdef _MSC_VER
#pragma once
#endif

#include <QThread>
#include <functional>
#include <thread>

namespace VTX
{
	namespace Worker
	{
		class BaseWorker : public QThread
		{
			Q_OBJECT

		  public:
			BaseWorker()		  = default;
			virtual ~BaseWorker() = default;

			void run() override { emit resultReady( this, _run() ); }

			virtual uint _run() = 0;

		  signals:
			void resultReady( BaseWorker *, const uint p_returnCode ) const;
			void updateProgress( BaseWorker *, const uint p_progress ) const;
			void logInfo( const std::string p_msg ) const;
			void logWarning( const std::string p_msg ) const;
			void logError( const std::string p_msg ) const;
			void logDebug( const std::string p_msg ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
