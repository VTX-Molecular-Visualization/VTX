#include "linearize_depth.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void LinearizeDepth::init( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::R32F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::NEAREST,
						 Wrapper::Texture2D::Filter::NEAREST );

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "LinearizeDepth",
														{ Util::FilePath( "shading/linearize_depth.frag" ) } );
	}

	void LinearizeDepth::resize( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );
	}

	void LinearizeDepth::render( const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassGeometric().getDepthTexture().bindToUnit( 0 );

		_program->use();

		/*
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam	 = p_scene.getCamera();
			const float				 camNear = cam.getNear();
			const float				 camFar	 = cam.getFar();
			// clipInfo.w: 0 = ortho ; 1 = perspective (always perspective for now).
			_program->setVec4f( "uClipInfo", camNear * camFar, camFar, camFar - camNear, 1.f );
		}

		p_renderer.getQuadVAO().drawArray( Wrapper::VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		*/
	}
} // namespace VTX::Renderer::GL::Pass
