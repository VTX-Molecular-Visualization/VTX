#include "renderer/gl/pass/linearize_depth.hpp"

namespace VTX::Renderer::GL::Pass
{
	void LinearizeDepth::init( const size_t p_width, const size_t p_height )
	{
		out.texture.create( p_width, p_height, GL_R32F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		/*
		_program = VTX_PROGRAM_MANAGER().createProgram( "LinearizeDepth",
														{ IO::FilePath( "shading/linearize_depth.frag" ) } );
														*/
	}

	void LinearizeDepth::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void LinearizeDepth::render()
	{
		assert( in.textureDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );

		in.textureDepth->bindToUnit( 0 );

		/*
		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam	 = p_scene.getCamera();
			const float				 camNear = cam.getNear();
			const float				 camFar	 = cam.getFar();

			_program->setVec4f( "uClipInfo", camNear * camFar, camFar, camFar - camNear, camNear );
			_program->setBool( "uIsPerspective", cam.isPerspective() );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/
	}
} // namespace VTX::Renderer::GL::Pass
