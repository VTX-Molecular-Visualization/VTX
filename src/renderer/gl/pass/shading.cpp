#include "shading.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Shading::~Shading()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void Shading::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_toonShading = p_programManager.createProgram( "ToonShading", { "shading/shading_toon.frag" } );
				_diffuseShading
					= p_programManager.createProgram( "DiffuseShading", { "shading/shading_diffuse.frag" } );
				_glossyShading = p_programManager.createProgram( "GlossyShading", { "shading/shading_glossy.frag" } );
				_flatShading   = p_programManager.createProgram( "FlatShading", { "shading/shading_flat.frag" } );

				// Use setting value.
				set();
			}

			void Shading::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void Shading::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D,
							   p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getColorsTexture() );

				glActiveTexture( GL_TEXTURE2 );
				// If SSAO/Blur disabled, texture is previoulsy cleared.
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassBlur().getTexture() );

				_currentShading->use();

				// TODO: do not update each frame
				const Color::Rgb & bgColor = VTX_SETTING().backgroundColor;
				glUniform3f( _uBackgroundColorLoc, bgColor.getR(), bgColor.getG(), bgColor.getB() );
				glUniform1f( _uFogNear, VTX_SETTING().fogNear );
				glUniform1f( _uFogFar, VTX_SETTING().fogFar );
				glUniform1f( _uFogDensity, VTX_SETTING().activeFog ? VTX_SETTING().fogDensity : 0.f );
				const Color::Rgb & fogColor = VTX_SETTING().fogColor;
				glUniform3f( _uFogColor, fogColor.getR(), fogColor.getG(), fogColor.getB() );
				// TODO: no need for flat shading
				// TODO: let the user choose where's the light
				// TODO: distinguish "view" and "world" lights
				const Vec4f & lightPosition
					= p_scene.getCamera().getViewMatrix() * Vec4f( p_scene.getCamera().getPosition(), 1.f );
				glUniform3f( _uLightPosition, lightPosition.x, lightPosition.y, lightPosition.z );
				const Color::Rgb & lightColor = VTX_SETTING().lightColor;
				glUniform3f( _uLightColor, lightColor.getR(), lightColor.getG(), lightColor.getB() );

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

			void Shading::set()
			{
				switch ( VTX_SETTING().shading )
				{
				case SHADING::TOON: _currentShading = _toonShading; break;
				case SHADING::GLOSSY: _currentShading = _glossyShading; break;
				case SHADING::FLAT_COLOR: _currentShading = _flatShading; break;
				case SHADING::DIFFUSE:
				default: _currentShading = _diffuseShading;
				}

				_currentShading->use();
				_uBackgroundColorLoc = glGetUniformLocation( _currentShading->getId(), "uBackgroundColor" );
				_uFogNear			 = glGetUniformLocation( _currentShading->getId(), "uFogNear" );
				_uFogFar			 = glGetUniformLocation( _currentShading->getId(), "uFogFar" );
				_uFogDensity		 = glGetUniformLocation( _currentShading->getId(), "uFogDensity" );
				_uFogColor			 = glGetUniformLocation( _currentShading->getId(), "uFogColor" );
				_uLightPosition		 = glGetUniformLocation( _currentShading->getId(), "uLightPosition" );
				_uLightColor		 = glGetUniformLocation( _currentShading->getId(), "uLightColor" );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
