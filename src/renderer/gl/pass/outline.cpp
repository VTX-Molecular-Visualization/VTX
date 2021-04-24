#include "outline.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const uint p_width, const uint p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::LINEAR,
						 Texture2D::Filter::LINEAR );

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { "shading/outline.frag" } );

		_program->use();

		const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
		/// TODO: use a value_ptr ?
		_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
	}

	void Outline::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void Outline::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassShading().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
			/// TODO: use a value_ptr ?
			_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}
} // namespace VTX::Renderer::GL::Pass
