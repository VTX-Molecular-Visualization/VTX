#include "worker_manager.hpp"
#include "define.hpp"
#include <thread>

namespace VTX
{
	namespace Worker
	{
		void WorkerManager::runWorker( BaseWorker * const p_woker ) { p_woker->work(); }

		void WorkerManager::runWorker( BaseWorker * const p_worker, const std::function<void( void )> * p_callback )
		{
			_workers.emplace( p_worker, p_callback );
			_current		 = p_worker;
			_threadException = nullptr;
			_thread			 = std::thread( [ & ]() {
				 try
				 {
					 _current->work();
				 }
				 catch ( const std::exception & )
				 {
					 VTX_ERROR( "Exception in thread" );
					 _threadException = std::current_exception();
				 }
			 } );
		}

		void WorkerManager::update( const double )
		{
			if ( _thread.joinable() )
			{
				VTX_DEBUG( "Thread joignable" );
				_thread.join();
				VTX_DEBUG( "Thread joined" );

				// Callback.
				if ( _threadException == false )
				{
					if ( ( _workers.at( _current ) ) == nullptr ) { VTX_DEBUG( "NULLPTR" ); }
					VTX_DEBUG( "Invoking thread callback" );
					//( *( _workers.at( _current ) ) )();
				}

				VTX_DEBUG( "Deleting worker" );
				// Delete callback.
				delete _workers.at( _current );

				// Clean members.
				_workers.erase( _current );
				_current = nullptr;

				// Rethrow async exception.
				if ( _threadException )
				{
					try
					{
						std::rethrow_exception( _threadException );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Catching thread exception" );
						VTX_ERROR( p_e.what() );
					}
				}
			}
		}
	} // namespace Worker
} // namespace VTX
