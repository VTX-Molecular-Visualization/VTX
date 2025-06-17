#ifndef __VTX_APP_CORE_THREADING_BASE_THREAD__
#define __VTX_APP_CORE_THREADING_BASE_THREAD__

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <util/callback.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Threading
{
	/**
	 * @brief Thread managed by ThreadingSystem
	 */
	class ThreadingSystem;

	class BaseThread
	{
	  public:
		using AsyncOp		   = std::function<uint( BaseThread & )>;
		using StoppableAsyncOp = std::function<uint( Util::StopToken, BaseThread & )>;
		using EndCallback	   = std::function<void( BaseThread &, uint )>;

	  public:
		/**
		 * @brief Default ctor. Never used directly. ThreadingSystem manager is responsible of the creation of
		 * BaseThread object.
		 */
		BaseThread( ThreadingSystem & p_manager ) : _manager( p_manager ) {};
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

		bool isFinished() const;
		inline std::jthread::id getId() const { return _thread.get_id(); }

		inline float getProgress() const { return _progress; }
		void		 setProgress( const float p_value );

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
		ThreadingSystem & _manager;

		std::jthread _thread;
		float		 _progress = 0.f;
		bool		 _stopped  = false;

		std::any _data;

		Util::StopToken _stopToken;
		void			_finish();
	};
} // namespace VTX::App::Core::Threading
#endif
