#include "linearize_depth.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	LinearizeDepth::~LinearizeDepth() { gl()->glDeleteFramebuffers( 1, &_fbo ); }

	void LinearizeDepth::init( const uint p_width, const uint p_height, const GL & )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::R32F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "LinearizeDepth", { "shading/linearize_depth.frag" } );
	}

	void LinearizeDepth::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );
	}

	void LinearizeDepth::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getDepthTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam	 = p_scene.getCamera();
			const float				 camNear = cam.getNear();
			const float				 camFar	 = cam.getFar();
			// clipInfo.w: 0 = ortho ; 1 = perspective (always perspective for now).
			_program->setVec4f( "uClipInfo", camNear * camFar, camFar, camFar - camNear, 1.f );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );

		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
} // namespace VTX::Renderer::GL::Pass
