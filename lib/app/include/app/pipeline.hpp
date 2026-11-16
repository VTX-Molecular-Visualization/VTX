#ifndef VTX_APP_PIPELINE_HPP
#define VTX_APP_PIPELINE_HPP

#include "app/pending_updates.hpp"
#include <exception>
#include <string>
#include <string_view>
#include <utility>

namespace VTX::App
{
	/**
	 * @brief Defines the main application pipeline steps and process them.
	 */
	struct Pipeline
	{
		/**
		 * @brief Process each step and log their duration.
		 */
		void process( E_PENDING_UPDATE p_pending, float p_delta, float p_elapsed );

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
