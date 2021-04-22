#include "fxaa.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	FXAA::~FXAA() {}

	void FXAA::init( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );

		_program = VTX_PROGRAM_MANAGER().createProgram( "AA", { "shading/fxaa.frag" } );
	}

	void FXAA::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );
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
