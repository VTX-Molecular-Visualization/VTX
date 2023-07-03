#ifndef __VTX_APP_OLD_WORKER__
#define __VTX_APP_OLD_WORKER__

#include "app/old/core/worker/base_thread.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include "app/old/core/worker/callback.hpp"
#include "app/old/manager/worker_manager.hpp"
#include <memory>
#include <type_traits>

namespace VTX
{
	inline void VTX_THREAD( App::Old::Core::Worker::BaseThread * const	  p_thread,
							App::Old::Core::Worker::CallbackThread * const p_callback = nullptr )
	{
		App::Old::Manager::WorkerManager::get().run( p_thread, p_callback );
	}
	inline void VTX_WORKER( App::Old::Core::Worker::BaseWorker * const	  p_worker,
							App::Old::Core::Worker::CallbackWorker * const p_callback = nullptr )
	{
		App::Old::Manager::WorkerManager::get().run( p_worker, p_callback );
	}
	// inline void VTX_ASYNC( const std::function<void( void )> & p_function ) { std::thread( p_function ).detach(); }
} // namespace VTX

#endif
