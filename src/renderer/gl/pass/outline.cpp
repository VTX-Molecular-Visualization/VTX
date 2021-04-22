#include "outline.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Outline::~Outline() { gl()->glDeleteFramebuffers( 1, &_fbo ); }

	void Outline::init( const uint p_width, const uint p_height, const GL & )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::LINEAR,
						 Texture2D::Filter::LINEAR );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { "shading/outline.frag" } );

		_program->use();

		const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
		/// TODO: use a value_ptr ?
		_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
	}

	void Outline::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );
		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );
	}

	void Outline::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, p_renderer.getPassShading().getTexture() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
			/// TODO: use a value_ptr ?
			_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
} // namespace VTX::Renderer::GL::Pass
