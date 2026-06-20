#ifndef __VTX_RENDERER_CONTEXT_GL_SAMPLER__
#define __VTX_RENDERER_CONTEXT_GL_SAMPLER__

#include "include_opengl.hpp"
#include <cassert>
#include <utility>

namespace VTX::Renderer::Context::Backend::GL
{
	class Sampler
	{
	  public:
		Sampler(
			const GLint p_wrappingS,
			const GLint p_wrappingT,
			const GLint p_minFilter,
			const GLint p_magFilter
		) noexcept
		{
			glCreateSamplers( 1, &_id );
			glSamplerParameteri( _id, GL_TEXTURE_WRAP_S, p_wrappingS );
			glSamplerParameteri( _id, GL_TEXTURE_WRAP_T, p_wrappingT );
			glSamplerParameteri( _id, GL_TEXTURE_WRAP_R, p_wrappingT );
			glSamplerParameteri( _id, GL_TEXTURE_MIN_FILTER, p_minFilter );
			glSamplerParameteri( _id, GL_TEXTURE_MAG_FILTER, p_magFilter );
		}

		Sampler( const Sampler & )			   = delete;
		Sampler & operator=( const Sampler & ) = delete;

		Sampler( Sampler && p_other ) noexcept : _id( std::exchange( p_other._id, GL_INVALID_INDEX ) ) {}

		Sampler & operator=( Sampler && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				destroy();
				_id = std::exchange( p_other._id, GL_INVALID_INDEX );
			}

			return *this;
		}

		~Sampler() noexcept { destroy(); }

		void destroy() noexcept
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteSamplers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline GLuint getId() const noexcept { return _id; }

		void bindToUnit( const GLuint p_unit ) const noexcept { glBindSampler( p_unit, _id ); }

		static void unbindFromUnit( const GLuint p_unit ) noexcept { glBindSampler( p_unit, 0 ); }

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};

} // namespace VTX::Renderer::Context::Backend::GL

#endif
