#ifndef __VTX_APP_WORKER__
#define __VTX_APP_WORKER__

#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include "app/core/worker/callback.hpp"
#include "app/manager/worker_manager.hpp"
#include <memory>
#include <type_traits>

namespace VTX
{
	inline void VTX_THREAD( App::Core::Worker::BaseThread * const	  p_thread,
							App::Core::Worker::CallbackThread * const p_callback = nullptr )
	{
		App::Manager::WorkerManager::get().run( p_thread, p_callback );
	}
	inline void VTX_WORKER( App::Core::Worker::BaseWorker * const	  p_worker,
							App::Core::Worker::CallbackWorker * const p_callback = nullptr )
	{
		App::Manager::WorkerManager::get().run( p_worker, p_callback );
	}
	// inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX

#endif
