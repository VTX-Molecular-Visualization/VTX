#ifndef __VTX_WORKER_MANAGER__
#define __VTX_WORKER_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_thread.hpp"
#include "base_worker.hpp"
#include "event/event_manager.hpp"
#include "generic/base_updatable.hpp"
#include "tool/logger.hpp"
#include <QMetaType>
#include <QThreadPool>
#include <string>

Q_DECLARE_METATYPE( std::string )

namespace VTX
{
	namespace Worker
	{
		using Callback = std::function<void( const uint )>;

		class WorkerManager final : public QObject
		{
			Q_OBJECT

		  public:
			inline static WorkerManager & get()
			{
				static WorkerManager instance;
				return instance;
			}

			void run( BaseThread * p_worker, Callback * const p_callback )
			{
				p_worker->setParent( this );
				_workers.emplace( p_worker, p_callback );
				connect( p_worker, &Worker::BaseThread::resultReady, this, &WorkerManager::_resultReady );
				connect( p_worker, &Worker::BaseThread::updateProgress, this, &WorkerManager::_updateProgress );
				connect( p_worker, &Worker::BaseThread::logInfo, this, &WorkerManager::_logInfo );
				connect( p_worker, &Worker::BaseThread::logWarning, this, &WorkerManager::_logWarning );
				connect( p_worker, &Worker::BaseThread::logError, this, &WorkerManager::_logError );
				connect( p_worker, &Worker::BaseThread::logDebug, this, &WorkerManager::_logDebug );
				connect( p_worker, &Worker::BaseThread::finished, p_worker, &QObject::deleteLater );
				VTX_DEBUG( "Starting thread" );
				p_worker->start();
			}

			void run( BaseWorker * p_worker ) { p_worker->run(); }

		  private:
			WorkerManager() { qRegisterMetaType<std::string>(); }
			WorkerManager( const WorkerManager & ) = delete;
			WorkerManager & operator=( const WorkerManager & ) = delete;
			~WorkerManager()
			{
				// Stop and delete all running threads.
				for ( const std::pair<BaseThread *, Callback *> & pair : _workers )
				{
					pair.first->quit();
					pair.first->wait();

					delete pair.second;
					delete pair.first;
				}
			}

			std::map<BaseThread *, Callback *> _workers = std::map<BaseThread *, Callback *>();

		  private slots:
			void _resultReady( BaseThread * p_worker, const uint p_returnCode )
			{
				VTX_DEBUG( "Thread finished: " + std::to_string( p_returnCode ) );

				assert( p_worker != nullptr );

				// Call callback and delete all.
				Callback * callback = _workers.at( p_worker );

				( *callback )( p_returnCode );
				delete callback;

				_workers.erase( _workers.find( p_worker ) );
				delete p_worker;
			}

			void _updateProgress( BaseThread * p_worker, const uint p_progress )
			{
				VTX_EVENT( new Event::VTXEventValue<float>( Event::Global::UPDATE_PROGRESS_BAR, p_progress ) );
			}

			void _logInfo( const std::string p_msg ) { VTX_INFO( p_msg ); }
			void _logWarning( const std::string p_msg ) { VTX_WARNING( p_msg ); }
			void _logError( const std::string p_msg ) { VTX_ERROR( p_msg ); }
			void _logDebug( const std::string p_msg ) { VTX_DEBUG( p_msg ); }
		};
	} // namespace Worker

	inline void VTX_WORKER( VTX::Worker::BaseThread * const p_worker, Worker::Callback * const p_callback )
	{
		Worker::WorkerManager::get().run( p_worker, p_callback );
	}
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker ) { Worker::WorkerManager::get().run( p_worker ); }
	inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX
#endif
