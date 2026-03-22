#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX
{
	namespace Util
	{
		/**
		 * @brief Log levels mapped to spdlog levels.
		 */
		enum class LOG_LEVEL : uint
		{
			LOG_TRACE	 = spdlog::level::trace,
			LOG_DEBUG	 = spdlog::level::debug,
			LOG_INFO	 = spdlog::level::info,
			LOG_WARNING	 = spdlog::level::warn,
			LOG_ERROR	 = spdlog::level::err,
			LOG_CRITICAL = spdlog::level::critical,
		};

		/**
		 * @brief Log categories for hints.
		 */
		enum class LOG_HINT : uint
		{
			STD,
			PY_IN,
			PY_OUT
		};

		/**
		 * @brief Struct containing log information, used for the onPrintLog callback.
		 */
		struct LogInfo
		{
			LOG_LEVEL	level;
			LOG_HINT	hint;
			std::string date;
			std::string message;
		};

		/**
		 * @brief Static async logger usig spdlog.
		 */
		class Logger final
		{
		  public:
			static void init( const FilePath & p_logPath, const bool p_debug = false );
			static const char * toSpdlogHint( const LOG_HINT p_hint );

			static void log( const LOG_LEVEL p_lvl, const LOG_HINT p_hint, std::string_view p_msg )
			{
				spdlog::log(
					spdlog::source_loc { "", 0, toSpdlogHint( p_hint ) }, spdlog::level::level_enum( p_lvl ), p_msg
				);
			}

			template<typename... Args>
			static void log(
				const LOG_LEVEL					  p_lvl,
				const LOG_HINT					  p_hint,
				const fmt::format_string<Args...> p_fmt,
				Args &&... p_args
			)
			{
				spdlog::log(
					spdlog::source_loc { "", 0, toSpdlogHint( p_hint ) },
					spdlog::level::level_enum( p_lvl ),
					p_fmt,
					std::forward<Args>( p_args )...
				);
			}

			static void flush();

			static void stop();

			/**
			 * @brief Triggered by spdlog (async).
			 */
			inline static Util::Callback<const LogInfo &> onLog;
		};

	} // namespace Util

	using LOGGER = Util::Logger;

	inline void VTX_LOG( const Util::LOG_LEVEL p_level, const Util::LOG_HINT p_hint, std::string_view p_msg )
	{
		LOGGER::log( p_level, p_hint, p_msg );
	}
	template<typename... Args>
	inline void VTX_LOG(
		const Util::LOG_LEVEL			  p_level,
		const Util::LOG_HINT			  p_hint,
		const fmt::format_string<Args...> p_fmt,
		Args &&... p_args
	)
	{
		LOGGER::log( p_level, p_hint, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_TRACE( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_TRACE, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_TRACE( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_TRACE, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_DEBUG( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_DEBUG, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_DEBUG( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_DEBUG, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_INFO( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_INFO( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_WARNING( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_WARNING, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_WARNING( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_WARNING, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_ERROR( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_ERROR, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_ERROR( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_ERROR, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_CRITICAL( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_CRITICAL, Util::LOG_HINT::STD, p_msg );
	}
	template<typename... Args>
	inline void VTX_CRITICAL( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_CRITICAL, Util::LOG_HINT::STD, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_PYTHON_IN( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::PY_IN, p_msg );
	}
	template<typename... Args>
	inline void VTX_PYTHON_IN( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::PY_IN, p_fmt, std::forward<Args>( p_args )... );
	}

	inline void VTX_PYTHON_OUT( const std::string_view p_msg )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::PY_OUT, p_msg );
	}
	template<typename... Args>
	inline void VTX_PYTHON_OUT( const fmt::format_string<Args...> p_fmt, Args &&... p_args )
	{
		LOGGER::log( Util::LOG_LEVEL::LOG_INFO, Util::LOG_HINT::PY_OUT, p_fmt, std::forward<Args>( p_args )... );
	}

} // namespace VTX

#endif
