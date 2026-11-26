#ifndef VTX_UTIL_THREAD_FUTURE_HPP
#define VTX_UTIL_THREAD_FUTURE_HPP

#include <chrono>
#include <future>

namespace VTX::Util::Thread
{
	/**
	 * @brief Check if a std::future is ready.
	 */
	template<typename T>
	bool isReady( const std::future<T> & p_future )
	{ return p_future.wait_for( std::chrono::seconds( 0 ) ) == std::future_status::ready; }
} // namespace VTX::Util::Thread

#endif
