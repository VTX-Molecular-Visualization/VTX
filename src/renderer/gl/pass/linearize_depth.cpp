#include "linearize_depth.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	LinearizeDepth::~LinearizeDepth()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_texture );
	}

	void LinearizeDepth::init( const uint p_width, const uint p_height, const GL & )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "LinearizeDepth", { "shading/linearize_depth.frag" } );
	}

	void LinearizeDepth::resize( const uint p_width, const uint p_height, const GL & )
	{
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
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
