#include "util/logger.hpp"
#include "util/chrono.hpp"
#include "util/exceptions.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace
{
	std::string pointTimeToStr( const std::chrono::system_clock::time_point & p_timePoint )
	{
		const std::string timePointStr( fmt::format( "{:%T}", p_timePoint ) );
		return timePointStr.substr( 0, 8 );
	}

	VTX::Util::LogInfo spdLogLogMsgToLogInfo( const spdlog::details::log_msg & p_msg )
	{
		return {
			VTX::Util::LOG_LEVEL( int( p_msg.level ) ),
			pointTimeToStr( p_msg.time ),
			std::string_view( p_msg.payload.begin(), std::distance( p_msg.payload.begin(), p_msg.payload.end() ) )
		};
	}
} // namespace

namespace VTX::Util
{
	void Logger::init( const std::filesystem::path & p_logDir, const bool p_debug )
	{
		try
		{
			spdlog::init_thread_pool( 8192, 1 );

			// Console sink.
			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			consoleSink->set_level( p_debug ? spdlog::level::trace : spdlog::level::info );

			// File sink.
			auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
				( p_logDir / ( std::to_string( Chrono::getTimestamp() ) + ".log" ) ).string()
			);
			fileSink->set_level( spdlog::level::trace );

			auto callbackSink = std::make_shared<spdlog::sinks::callback_sink_mt>(
				[]( const spdlog::details::log_msg & p_msg ) { onPrintLog( spdLogLogMsgToLogInfo( p_msg ) ); }
			);

			callbackSink->set_level( p_debug ? spdlog::level::debug : spdlog::level::info );

			// Logger.
			std::vector<spdlog::sink_ptr> sinks { consoleSink, fileSink, callbackSink };
			auto						  logger = std::make_shared<spdlog::async_logger>(
				 "vtx_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block
			 );
			logger->set_pattern( "[%t] [%H:%M:%S] [%^%l%$] %v" );
			logger->set_level( spdlog::level::trace );
			logger->flush_on( p_debug ? spdlog::level::trace : spdlog::level::info );

			spdlog::set_default_logger( logger );

			VTX_DEBUG( "Logger initialized" );
		}
		catch ( const spdlog::spdlog_ex & p_e )
		{
			throw LibException( "Logger initialization failed : " + std::string( p_e.what() ) );
		}
	}

	void Logger::flush() { spdlog::flush_on( spdlog::level::trace ); }
	void Logger::stop()
	{
		flush();
		onPrintLog.clear();
	}

} // namespace VTX::Util
