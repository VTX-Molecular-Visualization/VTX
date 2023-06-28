#include "app/old/render/renderer/gl/pass/fxaa.hpp"
#include "app/old/render/renderer/gl/gl.hpp"
#include "app/old/render/renderer/gl/program_manager.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
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

		_program = VTX_PROGRAM_MANAGER().createProgram( "AA", { FilePath( "shading/fxaa.frag" ) } );
	}

	void FXAA::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );
	}

	void FXAA::render( const App::Old::Application::Scene & p_scene, const GL & p_renderer )
	{
		p_renderer.getOutputFramebuffer().bind();

		p_renderer.getPassSelection().getTexture().bindToUnit( 0 );

		_program->use();

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

} // namespace VTX::App::Old::Render::Renderer::GL::Pass
