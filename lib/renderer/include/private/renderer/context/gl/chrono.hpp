#ifndef __VTX_RENDERER_CONTEXT_GL_CHRONO__
#define __VTX_RENDERER_CONTEXT_GL_CHRONO__

#include <cassert>
#include <util/chrono.hpp>

namespace VTX::Renderer::Context::GL
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

	inline const float CHRONO_GPU( const Util::Chrono::Task & p_task )
	{
		Chrono c;
		c.start();
		p_task();
		c.stop();
		return float( c.stop() );
	};
} // namespace VTX::Renderer::Context::GL

#endif
