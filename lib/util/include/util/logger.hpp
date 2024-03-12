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
			TRACE	 = spdlog::level::trace,
			DEBUG	 = spdlog::level::debug,
			INFO	 = spdlog::level::info,
			WARNING	 = spdlog::level::warn,
			ERROR	 = spdlog::level::err,
			CRITICAL = spdlog::level::critical
		};

		struct LogInfo
		{
			LOG_LEVEL	level;
			std::string date;
			std::string message;
		};

		class Logger final : public Generic::BaseStaticSingleton<Logger>
		{
		  public:
			explicit Logger( StructPrivacyToken p_token );
			Logger( std::initializer_list<int> ) = delete;

			void init( const FilePath & p_logPath = Filesystem::getExecutableDir() );

			template<typename... Args>
			void log( const LOG_LEVEL p_logLevel, const fmt::format_string<Args...> p_fmt, Args &&... p_args )
			{
				spdlog::log( spdlog::level::level_enum( p_logLevel ), p_fmt, std::forward<Args>( p_args )... );
			}

			void flush();

			Util::Callback<LogInfo> onPrintLog;
		};

	} // namespace Util

	template<typename... Args>
	inline void VTX_TRACE( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::TRACE, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_DEBUG( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::DEBUG, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_INFO( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::INFO, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_WARNING( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::WARNING, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_ERROR( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::ERROR, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_CRITICAL( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		Util::Logger::get().log( Util::LOG_LEVEL::CRITICAL, p_fmt, std::forward<Args>( p_args )... );
	}
} // namespace VTX

#endif
