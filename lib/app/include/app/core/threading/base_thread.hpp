#ifndef __VTX_APP_CORE_THREADING_BASE_THREAD__
#define __VTX_APP_CORE_THREADING_BASE_THREAD__

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Threading
{
	class ThreadingSystem;

	class BaseThread
	{
	  public:
		using AsyncOp	  = std::function<uint( BaseThread & )>;
		using EndCallback = std::function<void( BaseThread &, uint )>;

	  public:
		BaseThread( ThreadingSystem & p_manager ) : _manager( p_manager ) {};
		~BaseThread();

		void start( const AsyncOp & p_function );
		void start( const AsyncOp & p_function, const EndCallback & p_callback );

		void wait();
		void stop();

		bool isFinished() const;

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

		std::thread _thread;
		float		_progress = 0.f;
		bool		_stopped  = false;

		std::any _data;

		void _finish();
	};
} // namespace VTX::App::Core::Threading
#endif
