#ifndef VTX_APP_PIPELINE_HPP
#define VTX_APP_PIPELINE_HPP

#include <exception>
#include <string>
#include <string_view>
#include <util/hashing.hpp>
#include <utility>

namespace VTX::Util::Monitoring
{
	class Stats;
}

namespace VTX::App
{
	/**
	 * @brief Defines the main application pipeline steps and process them.
	 */
	struct Pipeline
	{
		inline static const Hash UPDATE_PASSES = Util::hash( "UPDATE_PASSES" );
		inline static const Hash UPDATE_EVENTS = Util::hash( "UPDATE_EVENTS" );
		inline static const Hash POST_UPDATE   = Util::hash( "POST_UPDATE" );
		inline static const Hash RENDER		   = Util::hash( "RENDER" );
		inline static const Hash POST_RENDER   = Util::hash( "POST_RENDER" );
		inline static const Hash LATE		   = Util::hash( "LATE" );

		/**
		 * @brief Process each step and log their duration.
		 */
		void process( Util::Monitoring::Stats &, float p_delta, float p_elapsed );

	  private:
		std::string _lastError;
		size_t		_lastErrorRepeatCount = 0;

		template<typename F>
		bool _step( const std::string_view p_step, F && p_function ) noexcept
		{
			try
			{
				std::forward<F>( p_function )();
				return true;
			}
			catch ( const std::exception & p_e )
			{
				_logError( p_step, p_e.what() );
			}
			catch ( ... )
			{
				_logError( p_step, "unknown exception" );
			}
			return false;
		}

		void _logError( std::string_view p_step, std::string_view p_error ) noexcept;
		void _flushRepeatedError() noexcept;
	};

} // namespace VTX::App

#endif
