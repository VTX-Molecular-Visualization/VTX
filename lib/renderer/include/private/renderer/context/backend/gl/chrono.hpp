#ifndef __VTX_RENDERER_CONTEXT_GL_CHRONO__
#define __VTX_RENDERER_CONTEXT_GL_CHRONO__

#include <cassert>

namespace VTX::Renderer::Context::Backend::GL
{
	class Chrono
	{
	  public:
		Chrono()
		{
			glCreateQueries( GL_TIMESTAMP, 1, &_queryStart );
			glCreateQueries( GL_TIMESTAMP, 1, &_queryEnd );

			assert( glIsQuery( _queryStart ) );
			assert( glIsQuery( _queryEnd ) );
		}

		~Chrono()
		{
			glDeleteQueries( 1, &_queryStart );
			glDeleteQueries( 1, &_queryEnd );
		}

		inline void start() { glQueryCounter( _queryStart, GL_TIMESTAMP ); }

		inline double stop()
		{
			glQueryCounter( _queryEnd, GL_TIMESTAMP );

			int32_t available = 0;
			while ( available != GL_TRUE )
			{
				glGetQueryObjectiv( _queryEnd, GL_QUERY_RESULT_AVAILABLE, &available );
			}

			uint64_t startTime, endTime;
			glGetQueryObjectui64v( _queryStart, GL_QUERY_RESULT, &startTime );
			glGetQueryObjectui64v( _queryEnd, GL_QUERY_RESULT, &endTime );

			return ( endTime - startTime ) * 1e-6;
		}

	  private:
		uint32_t _queryStart = 0;
		uint32_t _queryEnd	 = 0;
	};

	template<class F, class... Args>
		requires( std::is_void_v<std::invoke_result_t<F, Args...>> )
	inline float CHRONO_GPU( F && p_f, Args &&... p_args )
	{
		Chrono c;
		c.start();
		std::invoke( std::forward<F>( p_f ), std::forward<Args>( p_args )... );
		return float( c.stop() );
	}
} // namespace VTX::Renderer::Context::GL

#endif
