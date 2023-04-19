#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include "generic/base_static_singleton.hpp"
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>

namespace VTX
{
	namespace Util
	{
		class Logger final : public Generic::BaseStaticSingleton<Logger>
		{
		  public:
			Logger( StructPrivacyToken p_token );
			Logger( std::initializer_list<int> ) = delete;

			void init( const std::filesystem::path & );
		};

	} // namespace Util

	template<typename... Args>
	inline void VTX_TRACE( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::trace, p_fmt, p_args... );
	}

	template<typename... Args>
	inline void VTX_DEBUG( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::debug, p_fmt, p_args... );
	}

	template<typename... Args>
	inline void VTX_INFO( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::info, p_fmt, p_args... );
	}

	template<typename... Args>
	inline void VTX_WARNING( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::warn, p_fmt, p_args... );
	}

	template<typename... Args>
	inline void VTX_ERROR( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::err, p_fmt, p_args... );
	}

	template<typename... Args>
	inline void VTX_CRITICAL( const std::string & p_fmt, const Args &... p_args )
	{
		spdlog::log( spdlog::level::critical, p_fmt, p_args... );
	}
} // namespace VTX

#endif
