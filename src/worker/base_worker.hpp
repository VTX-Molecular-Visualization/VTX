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

			void run() override { emit resultReady( _run() ); }

			virtual uint _run() = 0;

		  signals:
			void resultReady( uint p_returnCode );
		};
	} // namespace Worker
} // namespace VTX
#endif
