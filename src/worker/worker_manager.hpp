#ifndef __VTX_WORKER_MANAGER__
#define __VTX_WORKER_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "generic/base_updatable.hpp"
#include <map>
#include <queue>
#include <vector>

namespace VTX
{
	namespace Worker
	{
		class WorkerManager : public Generic::BaseUpdatable
		{
			using MapWorkerPtrCallback = std::map<BaseWorker *, const std::function<void( void )> *>;
			using QueueWorkerPtr	   = std::queue<BaseWorker *>;
			using VectorThread		   = std::vector<std::thread>;

		  public:
			// Sync.
			void run( BaseWorker * const p_woker );
			// Async.
			void run( BaseWorker * const p_woker, const std::function<void( void )> * );

			virtual void update( const double p_deltaTime ) override;

		  private:
			// TODO: manages multiple threads and worker queue.
			MapWorkerPtrCallback _workers = MapWorkerPtrCallback();
			// QueueWorkerPtr		 _queue	  = QueueWorkerPtr();
			BaseWorker * _current = nullptr;

			// VectorThread		 _threads		  = VectorThread( std::thread::hardware_concurrency() - 1 );
			std::thread		   _thread			= std::thread();
			std::exception_ptr _threadException = nullptr;
		};
	} // namespace Worker
} // namespace VTX
#endif
