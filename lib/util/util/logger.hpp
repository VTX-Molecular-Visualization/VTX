#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include <filesystem>
#include <spdlog/spdlog.h>

namespace VTX::Util::Logger
{
	void init( const std::filesystem::path & p_logDir = std::filesystem::current_path() );
} // namespace VTX::Util::Logger

#define VTX_DEBUG( ... ) spdlog::debug( __VA_ARGS__ )
#define VTX_INFO( ... ) spdlog::info( __VA_ARGS__ )
#define VTX_WARNING( ... ) spdlog::warn( __VA_ARGS__ )
#define VTX_ERROR( ... ) spdlog::error( __VA_ARGS__ )
#define VTX_CRITICAL( ... ) spdlog::critical( __VA_ARGS__ )

#endif
