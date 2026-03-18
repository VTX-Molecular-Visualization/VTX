#ifndef __VTX_APP_THREADING_BASE_THREAD__
#define __VTX_APP_THREADING_BASE_THREAD__

#include <any>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <util/callback.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>

namespace VTX::App::Threading
{
	/**
	 * @brief Thread managed by ThreadManager
	 */
	class ThreadManager;

	class BaseThread
	{
	  public:
		using AsyncOp		   = std::function<uint( BaseThread & )>;
		using StoppableAsyncOp = std::function<uint( Util::StopToken, BaseThread & )>;
		using EndCallback	   = std::function<void( BaseThread &, uint, bool p_manuallyStopped )>;
		using ID			   = std::jthread::id;

	  public:
		/**
		 * @brief Default ctor. Never used directly. ThreadManager manager is responsible of the creation of
		 * BaseThread object.
		 */
		BaseThread( ThreadManager & p_manager ) : _manager( p_manager ) {};
		~BaseThread();

		/**
		 * @brief Starts the content of the function in a new thread.
		 * The thread is ended by the manager in the _finish method.
		 */
		void start( const AsyncOp & p_function );
		void start( const StoppableAsyncOp & p_function );
		/**
		 * @brief Starts the content of the function in a new thread.
		 * Once the function returns p_callback is called.
		 * The thread is ended by the manager in the _finish method.
		 */
		void start( const AsyncOp & p_function, const EndCallback & p_callback );
		void start( const StoppableAsyncOp & p_function, const EndCallback & p_callback );

		void wait();
		void stop();

		bool	  isFinished() const { return _finished.load( std::memory_order_relaxed ); }
		bool	  isManuallyStopped() const { return _stopped; }
		inline ID getId() const { return _thread.get_id(); }

		inline float getProgress() const { return _progress.load( std::memory_order_relaxed ); }
		void		 setProgress( const float p_value );

		std::string getProgressText() const;
		void		setProgressText( const std::string & p_text );

		Util::Callback<float> onProgress;

		template<typename T>
		T get() const
		{
			return std::any_cast<T>( _data );
		}
		template<typename T>
		void set( const T & p_data )
		{
			_data = p_data;
		}

	  private:
		ThreadManager & _manager;

		std::jthread		 _thread;
		std::atomic<float>	 _progress  = 0.f;
		std::atomic<bool>	 _finished  = false;
		bool				 _stopped   = false;

		mutable std::mutex _progressTextMutex;
		std::string		   _progressText;

		std::any _data;

		Util::StopToken _stopToken;
		void			_finish();
	};
} // namespace VTX::App::Threading
#endif
