#ifndef VTX_RENDERER_CONTEXT_EXECUTOR_NULL_HPP
#define VTX_RENDERER_CONTEXT_EXECUTOR_NULL_HPP

#include "renderer/context/command_buffer.hpp"

namespace VTX::Renderer::Context::Executor
{
	/**
	 * @brief Executor that does nothing.
	 */
	class Null
	{
	  public:
		/**
		 * @brief Execute a single command.
		 */
		template<typename PayLoad>
		void execute( const PayLoad & ) const noexcept
		{
			// Do nothing.
		}
	};
} // namespace VTX::Renderer::Context::Executor

#endif
