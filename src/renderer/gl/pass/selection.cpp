#include "selection.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Selection::init( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::LINEAR,
						 Texture2D::Filter::LINEAR );

		updateOutputFBO( p_renderer );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Selection", { "shading/selection.frag" } );

		_program->use();

		const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
		/// TODO: use a value_ptr ?
		_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
	}

	void Selection::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );

		updateOutputFBO( p_renderer );
	}

	void Selection::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		if ( VTX_SETTING().activeAA )
		{
			_fbo.bind();
		}
		else
		{
			gl()->glBindFramebuffer( GL_FRAMEBUFFER, p_renderer.getOutputFbo() );
		}

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		if ( VTX_SETTING().activeOutline )
		{
			p_renderer.getPassOutline().getTexture().bindToUnit( 1 );
		}
		else
		{
			p_renderer.getPassShading().getTexture().bindToUnit( 1 );
		}
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 2 );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			/// TODO: let the user define the line color.
			const Color::Rgb lineColor = Color::Rgb( 45, 243, 26 );
			/// TODO: use a value_ptr ?
			_program->setVec3f( "uLineColor", lineColor.getR(), lineColor.getG(), lineColor.getB() );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		if ( VTX_SETTING().activeAA == false )
		{
			gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}
	}

	void Selection::updateOutputFBO( const GL & p_renderer )
	{
		if ( VTX_SETTING().activeAA )
		{
			_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
		}
	}
} // namespace VTX::Renderer::GL::Pass
