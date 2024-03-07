#include "util/logger.hpp"
#include "util/chrono.hpp"
#include "util/exceptions.hpp"
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace VTX::Util
{
	Logger::Logger( StructPrivacyToken p_token ) {}

	void Logger::init( const std::filesystem::path & p_logDir )
	{
		try
		{
			spdlog::init_thread_pool( 8192, 1 );

			// Console sink.
			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef VTX_PRODUCTION
			consoleSink->set_level( spdlog::level::info );
#else
			consoleSink->set_level( spdlog::level::debug );
#endif

			// File sink.
			auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
				( p_logDir / ( std::to_string( Chrono::getTimestamp() ) + ".log" ) ).string()
			);
			fileSink->set_level( spdlog::level::trace );

			// Logger.
			std::vector<spdlog::sink_ptr> sinks { consoleSink, fileSink };
			auto						  logger = std::make_shared<spdlog::async_logger>(
				 "vtx_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block
			 );
			logger->set_pattern( "[%t] [%H:%M:%S] [%^%l%$] %v" );
			logger->set_level( spdlog::level::trace );
#ifdef VTX_PRODUCTION
			logger->flush_on( spdlog::level::info );
#else
			logger->flush_on( spdlog::level::trace );
#endif

			spdlog::set_default_logger( logger );

			VTX_INFO( "Logger initialized" );
		}
		catch ( const spdlog::spdlog_ex & p_e )
		{
			throw LibException( "Logger initialization failed : " + std::string( p_e.what() ) );
		}
	}

	void Logger::flush() { spdlog::flush_on( spdlog::level::trace ); }

} // namespace VTX::Util
