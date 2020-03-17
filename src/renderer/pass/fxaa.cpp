#include "fxaa.hpp"
#include "renderer/gl.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void FXAA::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				_aaShader = p_programManager.createProgram( "AA", { "shading/fxaa.frag" } );
			}

			void FXAA::clear() {}

			void FXAA::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassShading().getShadingTexture() );

				_aaShader->use();

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
