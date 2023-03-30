#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include <spdlog/spdlog.h>

namespace VTX
{
#define VTX_DEBUG( ... ) spdlog::debug( __VA_ARGS__ )
#define VTX_INFO( ... ) spdlog::info( __VA_ARGS__ )
#define VTX_WARNING( ... ) spdlog::warn( __VA_ARGS__ )
#define VTX_ERROR( ... ) spdlog::error( __VA_ARGS__ )
#define VTX_CRITICAL( ... ) spdlog::critical( __VA_ARGS__ )

	/*
	template<typename... Args>
	const auto VTX_DEBUG = spdlog::debug;
	template<typename... Args>
	const auto VTX_INFO = spdlog::info;
	template<typename... Args>
	const auto VTX_WARNING = spdlog::warn;
	template<typename... Args>
	const auto VTX_ERROR = spdlog::error;
	template<typename... Args>
	const auto VTX_CRITICAL = spdlog::critical;
	*/
} // namespace VTX

#endif
