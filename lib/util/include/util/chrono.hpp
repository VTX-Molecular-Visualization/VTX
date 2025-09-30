#ifndef __VTX_UTIL_CHRONO__
#define __VTX_UTIL_CHRONO__

#include <chrono>
#include <functional>
#include <string>

namespace VTX::Util
{
	class Chrono
	{
	  public:
		// using Timer = std::function<float( const Chrono::Task & )>;

		static long long getTimestamp() noexcept;

		void  start() noexcept;
		void  stop() noexcept;
		float elapsedTime() const noexcept;
		float intervalTime() noexcept;

	  private:
		using SystemClock = std::chrono::system_clock;
		using Clock		  = std::chrono::steady_clock;
		using Duration	  = std::chrono::duration<float, std::milli>;

		Clock::time_point _begin;
		Clock::time_point _interval;
		Clock::time_point _end;
	};

	template<class F, class... Args>
		requires( std::is_void_v<std::invoke_result_t<F, Args...>> )
	inline float CHRONO_CPU( F && p_f, Args &&... p_args )
	{
		Chrono c;
		c.start();
		std::invoke( std::forward<F>( p_f ), std::forward<Args>( p_args )... );
		c.stop();
		return c.elapsedTime();
	}
} // namespace VTX::Util
#endif
