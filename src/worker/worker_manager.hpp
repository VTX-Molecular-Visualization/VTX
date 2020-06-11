#ifndef __VTX_WORKER_MANAGER__
#define __VTX_WORKER_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "generic/base_updatable.hpp"

namespace VTX
{
	namespace Worker
	{
		class WorkerManager : public Generic::BaseUpdatable
		{
			using CallbackSuccess = std::function<void( void )>;
			using CallbackError	  = std::function<void( const std::exception & )>;

		  public:
			// Sync.
			void run( BaseWorker * const p_woker );
			// Async.
			void run( BaseWorker * const p_woker, const CallbackSuccess *, const CallbackError * );

			virtual void update( const double p_deltaTime ) override;

		  private:
			BaseWorker *			_worker = nullptr;
			const CallbackSuccess * _success;
			const CallbackError *	_error;
			std::exception_ptr		_threadException = nullptr;

			void _clean();
		};
	} // namespace Worker
} // namespace VTX
#endif
