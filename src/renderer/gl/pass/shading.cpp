#include "shading.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void Shading::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA16F, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_toonShading	= p_programManager.createProgram( "ToonShading", { "shading/toonShading.frag" } );
				_diffuseShading = p_programManager.createProgram( "DiffuseShading", { "shading/diffuseShading.frag" } );
				_blinnPhongShading
					= p_programManager.createProgram( "BlinnPhongShading", { "shading/blinnPhongShading.frag" } );
				_flatColorShading
					= p_programManager.createProgram( "FlatColorShading", { "shading/flatColorShading.frag" } );

				// Use setting value.
				set();
			}

			void Shading::clean()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void Shading::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getColorNormalCompressedTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getCamSpacePositionsTexture() );

				glActiveTexture( GL_TEXTURE2 );
				// If SSAO/Blur disabled, texture is previoulsy cleared.
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassBlur().getTexture() );

				_currentShading->use();

				glUniform1f( _uAoFactorLoc, Setting::Rendering::useSSAO ? Setting::Rendering::aoFactor : 0.f );

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

			void Shading::set()
			{
				switch ( Setting::Rendering::shading )
				{
				case SHADING::TOON: _currentShading = _toonShading; break;
				case SHADING::BLINN_PHONG: _currentShading = _blinnPhongShading; break;
				case SHADING::FLAT_COLOR: _currentShading = _flatColorShading; break;
				case SHADING::LAMBERT:
				default: _currentShading = _diffuseShading;
				}
				_uAoFactorLoc = glGetUniformLocation( _currentShading->getId(), "uAoFactor" );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
