#ifndef __VTX_APP_CORE_WORKER_BASE_THREAD__
#define __VTX_APP_CORE_WORKER_BASE_THREAD__

#include "app/core/callback_event.hpp"
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <util/types.hpp>

namespace VTX::App::Core::Worker
{
	class BaseThread
	{
	  public:
		using AsyncOp	  = std::function<uint( BaseThread & )>;
		using EndCallback = std::function<void( BaseThread &, uint )>;

	  public:
		BaseThread() = default;
		~BaseThread();

		void start( const AsyncOp & p_function );
		void start( const AsyncOp & p_function, const EndCallback & p_callback );

		void wait();
		void stop();

		bool isFinished() const;

		inline float getProgress() const { return _progress; }
		void		 setProgress( const float p_value );

		App::Core::CallbackEmitter<float> progressCallback = App::Core::CallbackEmitter<float>();

		template<typename T>
		T getData() const
		{
			return std::any_cast<T>( _data );
		}
		template<typename T>
		void setData( const T & p_data )
		{
			_data = p_data;
		}

	  private:
		std::thread _thread;
		float		_progress = 0.f;
		bool		_stopped  = false;

		std::any _data;

		void _finish();
	};
} // namespace VTX::App::Core::Worker
#endif