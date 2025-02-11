#ifndef __VTX_APP_CORE_THREADING_MANAGER__
#define __VTX_APP_CORE_THREADING_MANAGER__

#include "app/core/threading/base_thread.hpp"
#include <list>
#include <memory>
#include <util/singleton.hpp>

namespace VTX::App::Core::Threading
{
	class ThreadingSystem
	{
		friend class BaseThread;

	  public:
		ThreadingSystem() {}
		ThreadingSystem( const ThreadingSystem & )			   = delete;
		ThreadingSystem & operator=( const ThreadingSystem & ) = delete;
		~ThreadingSystem();

		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp );
		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp, const BaseThread::EndCallback & p_callback );

		void lateUpdate();

	  private:
		std::list<std::shared_ptr<BaseThread>> _threads			= std::list<std::shared_ptr<BaseThread>>();
		std::list<std::shared_ptr<BaseThread>> _stoppingThreads = std::list<std::shared_ptr<BaseThread>>();

		void _killThread( const BaseThread & p_thread );
		void _clearStoppedThreads();

		std::list<std::shared_ptr<BaseThread>>::const_iterator _findPtrFromThread( const BaseThread & p_thread ) const;
	};

} // namespace VTX::App::Core::Threading

namespace VTX::App
{
	// Access to the worker manager class in order to launch thread.
	inline Core::Threading::ThreadingSystem & THREADING_SYSTEM()
	{
		return Util::Singleton<Core::Threading::ThreadingSystem>::get();
	}
} // namespace VTX::App

#endif
