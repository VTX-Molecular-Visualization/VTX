#include "fxaa.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	FXAA::~FXAA() { gl()->glDeleteTextures( 1, &_texture ); }

	void FXAA::init( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		_program = VTX_PROGRAM_MANAGER().createProgram( "AA", { "shading/fxaa.frag" } );
	}

	void FXAA::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );
	}

	void FXAA::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, p_renderer.getOutputFbo() );

		gl()->glBindTextureUnit( 0, p_renderer.getPassSelection().getTexture() );

		_program->use();

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	}

} // namespace VTX::Renderer::GL::Pass
