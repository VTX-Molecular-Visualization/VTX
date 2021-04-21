#ifndef __VTX_PASS_SHADING__
#define __VTX_PASS_SHADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Shading : public BasePass
	{
	  public:
		Shading( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
		virtual ~Shading();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;
		void set();

		inline const GLuint & getTexture() const { return _texture; }
		inline const GLuint & getFbo() const { return _fbo; }

	  private:
		Program * _diffuseShading = nullptr;
		Program * _glossyShading  = nullptr;
		Program * _toonShading	  = nullptr;
		Program * _flatShading	  = nullptr;
		GLuint	  _fbo			  = GL_INVALID_VALUE;
		GLuint	  _texture		  = GL_INVALID_VALUE;

		GLint _uBackgroundColorLoc = GL_INVALID_INDEX;
		// Fog.
		GLint _uFogColor = GL_INVALID_INDEX;
		// Lights.
		GLint _uLightPosition = GL_INVALID_INDEX;
		GLint _uLightColor	  = GL_INVALID_INDEX;

		Program * _currentShading = nullptr;
	};

} // namespace VTX::Renderer::GL::Pass

#endif
