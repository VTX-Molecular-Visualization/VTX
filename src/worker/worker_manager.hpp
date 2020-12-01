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
		using CallbackSuccess = std::function<void( void )>;
		using CallbackError	  = std::function<void( const std::exception & )>;

		class WorkerManager final : public Generic::BaseUpdatable
		{
		  public:
			inline static WorkerManager & get()
			{
				static WorkerManager instance;
				return instance;
			}

			// Sync.
			void run( BaseWorker * const p_woker );
			// Async.
			void run( BaseWorker * const p_woker, const CallbackSuccess * const, const CallbackError * const );

			virtual void update( const double & p_deltaTime ) override;

		  private:
			BaseWorker *			_worker			 = nullptr;
			const CallbackSuccess * _success		 = nullptr;
			const CallbackError *	_error			 = nullptr;
			std::exception_ptr		_threadException = nullptr;

			WorkerManager()						   = default;
			WorkerManager( const WorkerManager & ) = delete;
			WorkerManager & operator=( const WorkerManager & ) = delete;
			~WorkerManager()								   = default;

			void _clean();
		};
	} // namespace Worker

	// TODO: will be deleted when all workers will be threaded.
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker ) { Worker::WorkerManager::get().run( p_worker ); }
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker, const Worker::CallbackSuccess * const p_success, const Worker::CallbackError * const p_error )
	{
		Worker::WorkerManager::get().run( p_worker, p_success, p_error );
	}
	inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX
#endif
