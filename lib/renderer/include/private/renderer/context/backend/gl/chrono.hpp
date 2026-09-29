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

		void start() const { glQueryCounter( _queryStart, GL_TIMESTAMP ); }

		double stop() const
		{
			glQueryCounter( _queryEnd, GL_TIMESTAMP );

			int32_t available = 0;
			while ( available != GL_TRUE )
			{
				glGetQueryObjectiv( _queryEnd, GL_QUERY_RESULT_AVAILABLE, &available );
			}

			uint64_t startTime = 0;
			uint64_t endTime   = 0;
			glGetQueryObjectui64v( _queryStart, GL_QUERY_RESULT, &startTime );
			glGetQueryObjectui64v( _queryEnd, GL_QUERY_RESULT, &endTime );

			return static_cast<double>( endTime - startTime ) * 1e-6;
		}

	  private:
		uint32_t _queryStart = 0;
		uint32_t _queryEnd	 = 0;
	};

	template<class F, class... Args>
		requires( std::is_void_v<std::invoke_result_t<F, Args...>> )
	inline float CHRONO_GPU( F && p_f, Args &&... p_args )
	{
		const Chrono c;
		c.start();
		std::invoke( std::forward<F>( p_f ), std::forward<Args>( p_args )... );
		return float( c.stop() );
	}
} // namespace VTX::Renderer::Context::Backend::GL

#endif
