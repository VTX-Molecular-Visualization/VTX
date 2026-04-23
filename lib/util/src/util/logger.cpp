#include "util/logger.hpp"
#include "util/chrono.hpp"
#include "util/enum.hpp"
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
	constexpr std::string_view NAME = "vtx_logger";

	std::string _pointTimeToStr( const std::chrono::system_clock::time_point & p_timePoint )
	{
		const std::string timePointStr( fmt::format( "{:%T}", p_timePoint ) );
		return timePointStr.substr( 0, 8 );
	}

	VTX::Util::LOG_HINT _toLogHint( const spdlog::source_loc & p_source )
	{
		const std::string_view hint = p_source.funcname != nullptr ? p_source.funcname : "";
		return hint.empty() ? VTX::Util::LOG_HINT::STD : VTX::Util::Enum::enumCast<VTX::Util::LOG_HINT>( std::string( hint ) );
	}

	VTX::Util::LogInfo _toLogInfo( const spdlog::details::log_msg & p_msg )
	{
		return { static_cast<VTX::Util::LOG_LEVEL>( p_msg.level ),
				 _toLogHint( p_msg.source ),
				 _pointTimeToStr( p_msg.time ),
				 std::string( p_msg.payload.begin(), p_msg.payload.end() ) };
	}

} // namespace

namespace VTX::Util
{
	const char * Logger::toSpdlogHint( const LOG_HINT p_hint )
	{
		return Enum::enumName( p_hint ).data();
	}

	void Logger::init( const std::filesystem::path & p_logDir, const bool p_debug )
	{
		try
		{
			spdlog::init_thread_pool( 8192, 1 );
			std::filesystem::create_directories( p_logDir );

			// File sink.
			auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
				( p_logDir / ( std::to_string( Chrono::getTimestamp() ) + ".log" ) ).string()
			);
			fileSink->set_level( spdlog::level::trace );

			// Callback sink.
			auto callbackSink
				= std::make_shared<spdlog::sinks::callback_sink_mt>( []( const spdlog::details::log_msg & p_msg )
																	 { onLog( _toLogInfo( p_msg ) ); } );
			callbackSink->set_level( p_debug ? spdlog::level::debug : spdlog::level::info );

			// Logger.
			std::vector<spdlog::sink_ptr> sinks { fileSink, callbackSink };

			// Console sink.
			if ( p_debug )
			{
				auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
				consoleSink->set_level( p_debug ? spdlog::level::trace : spdlog::level::info );
				sinks.push_back( consoleSink );
			}

			auto logger = std::make_shared<spdlog::async_logger>(
				NAME.data(), sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block
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

	void Logger::flush()
	{
		if ( auto logger = spdlog::get( NAME.data() ) )
		{
			logger->flush();
		}
	}

	void Logger::stop()
	{
		spdlog::shutdown();
		onLog.clear();
	}

} // namespace VTX::Util
