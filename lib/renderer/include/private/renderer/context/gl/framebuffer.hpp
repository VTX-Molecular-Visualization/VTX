#ifndef __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__
#define __VTX_RENDERER_CONTEXT_GL_FRAMEBUFFER__

#include "include_opengl.hpp"
#include "texture_2d.hpp"
#include <cassert>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	class Framebuffer
	{
	  public:
		Framebuffer()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateFramebuffers( 1, &_id );

			assert( glIsFramebuffer( _id ) );
		}

		~Framebuffer()
		{
			assert( _target == 0 );

			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteFramebuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline uint32_t getId() const { return _id; }

		inline void bind( const uint32_t p_target = GL_FRAMEBUFFER )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindFramebuffer( _target, _id );
		}

		inline static void bindDefault( const uint32_t p_id, const uint32_t p_target = GL_FRAMEBUFFER )
		{
			assert( p_target != 0 );

			glBindFramebuffer( p_target, p_id );
		}

		inline void unbind()
		{
			assert( _target != 0 );

			glBindFramebuffer( _target, 0 );
			_target = 0;
		}

		inline static void unbindDefault( const uint32_t p_target = GL_FRAMEBUFFER )
		{
			assert( p_target != 0 );

			glBindFramebuffer( p_target, 0 );
		}

		inline void attachTexture( const Texture2D & p_texture, const uint32_t p_attachment, const int32_t p_level = 0 )
		{
			assert( glIsFramebuffer( _id ) );
			assert( glIsTexture( p_texture.getId() ) );

			glNamedFramebufferTexture( _id, p_attachment, p_texture.getId(), p_level );
		}

		inline void setDrawBuffers( const std::vector<uint32_t> & p_drawBuffers )
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferDrawBuffers(
				_id, int32_t( p_drawBuffers.size() ), static_cast<const uint32_t *>( p_drawBuffers.data() )
			);
		}

		inline void setReadBuffer( const uint32_t p_readBuffer )
		{
			assert( glIsFramebuffer( _id ) );

			glNamedFramebufferReadBuffer( _id, p_readBuffer );
		}

	  private:
		uint32_t _id	 = GL_INVALID_INDEX;
		uint32_t _target = 0;
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_FRAMEBUFFER__
