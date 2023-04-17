#ifndef __VTX_WORKER_MANAGER__
#define __VTX_WORKER_MANAGER__

#include "base_thread.hpp"
#include "base_worker.hpp"
#include "app/old_app/event/event_manager.hpp"
#include "app/old_app/generic/base_updatable.hpp"
// #include <QMetaType>
//  #include <QThreadPool>
#include <string>
#include <util/logger.hpp>

// TODO reimplemente that without Qt
namespace VTX
{
	namespace Worker
	{
		using CallbackThread = std::function<void( uint )>;
		using CallbackWorker = std::function<void( void )>;
		using CallbackError	 = std::function<void( const std::exception & )>;

		class WorkerManager final // : public QObject
		{
			// Q_OBJECT

		  public:
			inline static WorkerManager & get()
			{
				static WorkerManager instance;
				return instance;
			}

			void run( BaseThread * p_worker, CallbackThread * const p_callback = nullptr )
			{
				// p_worker->setParent( this );
				_workers.emplace( p_worker, p_callback );

				// connect( p_worker, &Worker::BaseThread::resultReady, this, &WorkerManager::_resultReady );
				// connect( p_worker, &Worker::BaseThread::updateProgress, this, &WorkerManager::_updateProgress );
				// connect( p_worker, &Worker::BaseThread::logInfo, this, &WorkerManager::_logInfo );
				// connect( p_worker, &Worker::BaseThread::logWarning, this, &WorkerManager::_logWarning );
				// connect( p_worker, &Worker::BaseThread::logError, this, &WorkerManager::_logError );
				// connect( p_worker, &Worker::BaseThread::logDebug, this, &WorkerManager::_logDebug );
				//// connect( p_worker, &Worker::BaseThread::logFile, this, &WorkerManager::_logFile );
				// connect( p_worker, &Worker::BaseThread::finished, p_worker, &QObject::deleteLater );
				VTX_DEBUG( "Starting thread" );
				// p_worker->start();
			}

			void run( BaseWorker * p_worker, CallbackWorker * const p_callback = nullptr )
			{
				p_worker->run();
				if ( p_callback != nullptr )
				{
					( *p_callback )();
					delete p_callback;
				}
				delete p_worker;
			}

			void stopAll()
			{
				// Stop and delete all running threads.
				for ( const std::pair<BaseThread * const, CallbackThread *> & pair : _workers )
				{
					pair.first->quit();
					pair.first->wait();

					delete pair.second;
					delete pair.first;
				}

				_workers.clear();
			}

		  private:
			WorkerManager()
			{ /*qRegisterMetaType<std::string>();*/
			}
			WorkerManager( const WorkerManager & )			   = delete;
			WorkerManager & operator=( const WorkerManager & ) = delete;
			~WorkerManager() { stopAll(); }

			std::map<BaseThread *, CallbackThread *> _workers = std::map<BaseThread *, CallbackThread *>();

			// private slots:
			// void _resultReady( BaseThread * p_worker, const uint p_returnCode )
			//{
			//	VTX_DEBUG( "Thread finished: " + std::to_string( p_returnCode ) );

			//	assert( p_worker != nullptr );

			//	// Call callback and delete all.
			//	if ( _workers.find( p_worker ) != _workers.end() )
			//	{
			//		CallbackThread * const callback = _workers[ p_worker ];

			//		_workers.erase( _workers.find( p_worker ) );

			//		if ( callback != nullptr )
			//		{
			//			( *callback )( p_returnCode );
			//			delete callback;
			//		}
			//	}

			//	delete p_worker;
			//}

			// void _updateProgress( BaseThread * p_worker, const uint p_progress )
			//{
			//	VTX_EVENT( new Event::VTXEventValue<float>( Event::Global::UPDATE_PROGRESS_BAR, p_progress ) );
			// }

			// void _logInfo( const std::string p_msg ) { VTX_INFO( p_msg ); }
			// void _logWarning( const std::string p_msg ) { VTX_WARNING( p_msg ); }
			// void _logError( const std::string p_msg ) { VTX_ERROR( p_msg ); }
			// void _logDebug( const std::string p_msg ) { VTX_DEBUG( p_msg ); }
			//  void _logFile( const std::string p_msg ) { VTX_LOG_FILE( p_msg ); }
		};
	} // namespace Worker

	inline void VTX_THREAD( Worker::BaseThread * const p_thread, Worker::CallbackThread * const p_callback = nullptr )
	{
		Worker::WorkerManager::get().run( p_thread, p_callback );
	}
	inline void VTX_WORKER( Worker::BaseWorker * const p_worker, Worker::CallbackWorker * const p_callback = nullptr )
	{
		Worker::WorkerManager::get().run( p_worker, p_callback );
	}
	// inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX
#endif
