#ifndef __VTX_WORKER_MANAGER__
#define __VTX_WORKER_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "generic/base_updatable.hpp"
#include "tool/logger.hpp"
#include <QThreadPool>

namespace VTX
{
	namespace Worker
	{
		class WorkerManager final : public QObject
		{
			Q_OBJECT

		  public:
			inline static WorkerManager & get()
			{
				static WorkerManager instance;
				return instance;
			}

			void run( BaseWorker * const p_worker )
			{
				p_worker->setParent( this );

				connect( p_worker, &Worker::BaseWorker::resultReady, this, []( uint p_returnCode ) {
					VTX_DEBUG( "RETURN CODE: " + std::to_string( p_returnCode ) );
				} );
				// connect( p_worker, &Worker::BaseWorker::finished, this, &QObject::deleteLater );

				VTX_DEBUG( "Start" );
				p_worker->start();
			}

		  private:
			WorkerManager()						   = default;
			WorkerManager( const WorkerManager & ) = delete;
			WorkerManager & operator=( const WorkerManager & ) = delete;
			~WorkerManager()								   = default;
		};
	} // namespace Worker

	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker ) { Worker::WorkerManager::get().run( p_worker ); }
	inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX
#endif
