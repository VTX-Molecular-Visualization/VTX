#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include "filesystem.hpp"
#include "generic/base_static_singleton.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <util/callback.hpp>

namespace VTX
{
	namespace Util
	{
		enum class LOG_LEVEL : int
		{
			LOG_TRACE	 = spdlog::level::trace,
			LOG_DEBUG	 = spdlog::level::debug,
			LOG_INFO	 = spdlog::level::info,
			LOG_WARNING	 = spdlog::level::warn,
			LOG_ERROR	 = spdlog::level::err,
			LOG_CRITICAL = spdlog::level::critical
		};

		// Log info sent by callback
		struct LogInfo
		{
			LOG_LEVEL	level;
			std::string date;
			std::string message;
		};

		class Logger final
		{
			friend class Generic::UniqueInstance<Logger>;

		  public:
			Logger( std::initializer_list<int> ) = delete;

			void init( const FilePath & p_logPath = Filesystem::getExecutableDir(), const bool p_debug = false );

			template<typename... Args>
			void log( const LOG_LEVEL p_logLevel, const fmt::format_string<Args...> p_fmt, Args &&... p_args )
			{
				spdlog::log( spdlog::level::level_enum( p_logLevel ), p_fmt, std::forward<Args>( p_args )... );
			}

			void flush();
			void stop();

			Util::Callback<LogInfo> onPrintLog;

		  private:
			Logger();
		};

	} // namespace Util

	inline Util::Logger & LOGGER() { return Util::Generic::UniqueInstance<Util::Logger>::get(); }

	template<typename... Args>
	inline void VTX_TRACE( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_TRACE, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_DEBUG( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_DEBUG, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_INFO( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_INFO, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_WARNING( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_WARNING, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_ERROR( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_ERROR, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_CRITICAL( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER().log( Util::LOG_LEVEL::LOG_CRITICAL, p_fmt, std::forward<Args>( p_args )... );
	}
} // namespace VTX

#endif
