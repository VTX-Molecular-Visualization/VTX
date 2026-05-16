#ifndef __VTX_RENDERER_CONTEXT_EXECUTOR_NULL__
#define __VTX_RENDERER_CONTEXT_EXECUTOR_NULL__

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
		inline void execute( const PayLoad & ) const noexcept
		{
			// Do nothing.
		}
	};
} // namespace VTX::Renderer::Context::Executor

#endif
