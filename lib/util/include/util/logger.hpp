#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include "filesystem.hpp"
#include "generic/base_static_singleton.hpp"
#include <memory>
#include <spdlog/spdlog.h>

namespace VTX
{
	namespace Util
	{
		class Logger final : public Generic::BaseStaticSingleton<Logger>
		{
		  public:
			explicit Logger( StructPrivacyToken p_token );
			Logger( std::initializer_list<int> ) = delete;

			void init( const FilePath & p_logPath = Filesystem::getExecutableDir() );
			// void flush();
		};

	} // namespace Util

	template<typename... Args>
	inline void VTX_TRACE( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::trace, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_DEBUG( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::debug, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_INFO( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::info, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_WARNING( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::warn, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_ERROR( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::err, p_fmt, std::forward<Args>( p_args )... );
	}

	template<typename... Args>
	inline void VTX_CRITICAL( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		spdlog::log( spdlog::level::critical, p_fmt, std::forward<Args>( p_args )... );
	}
} // namespace VTX

#endif
