#include "worker_manager.hpp"
#include "define.hpp"
#include "event/event_manager.hpp"
#include "util/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void WorkerManager::run( BaseWorker * const p_woker ) { p_woker->work(); }

		void WorkerManager::run( BaseWorker * const p_worker, const std::function<void( void )> * p_callback )
		{
			_workers.emplace( p_worker, p_callback );
			_current		   = p_worker;
			_threadException   = nullptr;
			std::thread thread = std::thread( [ & ]() {
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

			thread.detach();
		}

		void WorkerManager::update( const double )
		{
			if ( _current == nullptr )
			{
				return;
			}

			if ( _current->isFinished() )
			{
				VTX_EVENT( new Event::VTXEventFloat( Event::Global::UPDATE_PROGRESS_BAR, 1.f ) );

				// Callback.
				if ( _threadException == false )
				{
					VTX_DEBUG( "Invoking thread callback" );
					( *( _workers.at( _current ) ) )();
				}

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
			else
			{
				VTX_EVENT( new Event::VTXEventFloat( Event::Global::UPDATE_PROGRESS_BAR, _current->getProgress() ) );
			}
		}
	} // namespace Worker
} // namespace VTX
