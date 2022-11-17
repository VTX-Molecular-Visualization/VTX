#include "fxaa.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	FXAA::~FXAA() {}

	void FXAA::init( const size_t p_width, const size_t p_height, const GL & p_renderer )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::RGBA16F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::NEAREST,
						 Wrapper::Texture2D::Filter::NEAREST );

		_program = VTX_PROGRAM_MANAGER().createProgram( "AA", { Util::FilePath( "shading/fxaa.frag" ) } );
	}

	void FXAA::resize( const size_t p_width, const size_t p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );
	}

	void FXAA::render( const GL & p_renderer )
	{
		p_renderer.getOutputFramebuffer().bind();

		p_renderer.getPassSelection().getTexture().bindToUnit( 0 );

		_program->use();

		p_renderer.getQuadVAO().drawArray( Wrapper::VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

} // namespace VTX::Renderer::GL::Pass
