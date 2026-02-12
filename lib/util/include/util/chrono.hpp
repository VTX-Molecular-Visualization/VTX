#ifndef __VTX_UTIL_CHRONO__
#define __VTX_UTIL_CHRONO__

#include "util/logger.hpp"
#include "util/string.hpp"
#include <chrono>
#include <functional>
#include <string>

namespace VTX::Util
{
	/**
	 * @brief Provides utility functions for measuring time intervals using a steady clock.
	 * Measures time in milliseconds with floating-point precision.
	 */
	class Chrono
	{
	  public:
		using Clock		= std::chrono::steady_clock;
		using Duration	= std::chrono::duration<float, std::milli>;
		using TimePoint = Clock::time_point;
		using TimeUnit	= std::chrono::milliseconds;

		/**
		 * @brief Returns the current timestamp.
		 */
		inline static long long getTimestamp() noexcept
		{
			return std::chrono::duration_cast<TimeUnit>( now().time_since_epoch() ).count();
		}

		/**
		 * @brief Returns the current time point.
		 */
		inline static TimePoint now() noexcept { return Clock::now(); }

		/**
		 * @brief Calculates the elapsed time between two time points.
		 */
		inline static float elapsedTime( const TimePoint p_start, const TimePoint p_end = now() ) noexcept
		{
			return std::chrono::duration_cast<Duration>( p_end - p_start ).count();
		}

		/**
		 * @brief Start or restart the chrono.
		 */
		inline void start() noexcept { _begin = _interval = now(); }

		/**
		 * @brief Time since start.
		 */
		inline float elapsedTime() const noexcept { return elapsedTime( _begin ); }

		/**
		 * @brief Time since last call.
		 */
		inline float intervalTime() noexcept
		{
			float intervalTime = elapsedTime( _interval );
			_interval		   = now();
			return intervalTime;
		}

	  private:
		TimePoint _begin;
		TimePoint _interval;
	};

	/**
	 * @brief Utility function to measure the execution time of a callable.
	 */
	template<class F, class... Args>
		requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, void>
	inline float CHRONO_CPU( F && p_f, Args &&... p_args )
	{
		Chrono c;
		c.start();
		std::invoke( std::forward<F>( p_f ), std::forward<Args>( p_args )... );
		return c.elapsedTime();
	}

	/**
	 * @brief Run a chrono during its lifetime.
	 */
	class ScopedChrono : public Chrono
	{
	  public:
		ScopedChrono( const std::string & p_name, const Util::LOG_LEVEL p_level = Util::LOG_LEVEL::LOG_INFO ) :
			_name( p_name ), _level( p_level )
		{
			start();
		}
		~ScopedChrono()
		{
			VTX_LOG( _level, "Scoped chrono '{}' : {}", _name, Util::String::durationToStr( elapsedTime() ) );
		}

	  private:
		std::string		_name;
		Util::LOG_LEVEL _level;
	};
} // namespace VTX::Util
#endif
