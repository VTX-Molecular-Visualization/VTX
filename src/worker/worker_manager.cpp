#include "worker_manager.hpp"
#include "define.hpp"
#include "event/event_manager.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Worker
	{
		void WorkerManager::run( BaseWorker * const p_woker ) { p_woker->work(); }

		void WorkerManager::run( BaseWorker * const p_worker, const CallbackSuccess * const p_success, const CallbackError * const p_error )
		{
			_worker			 = p_worker;
			_success		 = p_success;
			_error			 = p_error;
			_threadException = nullptr;

			std::thread thread = std::thread( [ & ]() {
				try
				{
					_worker->work();
				}
				// Catch exception ptr.
				catch ( const std::exception & )
				{
					_threadException = std::current_exception();
				}
			} );

			// Detach to not block main thread.
			thread.detach();
		}

		void WorkerManager::update( const double & )
		{
			if ( _worker == nullptr )
			{
				return;
			}

			// Rethrow async exception to catch it there and pass it to the error callback.
			if ( _threadException )
			{
				try
				{
					std::rethrow_exception( _threadException );
				}
				catch ( const std::exception & p_e )
				{
					( *_error )( p_e );
					_clean();
					return;
				}
			}

			if ( _worker->isFinished() )
			{
				VTX_EVENT( new Event::VTXEventValue<float>( Event::Global::UPDATE_PROGRESS_BAR, 1.f ) );
				( *_success )();
				_clean();
			}
			else
			{
				VTX_EVENT( new Event::VTXEventValue<float>( Event::Global::UPDATE_PROGRESS_BAR, _worker->getProgress() ) );
			}
		}

		void WorkerManager::_clean()
		{
			delete _success;
			delete _error;
			_worker = nullptr;
		}
	} // namespace Worker
} // namespace VTX
