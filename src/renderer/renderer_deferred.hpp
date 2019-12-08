#ifndef __VTX_RENDERER_DEFERRED__
#define __VTX_RENDERER_DEFERRED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class RendererDeferred : public BaseRenderer
		{
		  public:
			RendererDeferred()	= default;
			~RendererDeferred() = default;

			virtual void init( Object3D::Scene & ) override;
			virtual void clear( Object3D::Scene & ) override;
			virtual void render( Object3D::Scene &, const uint ) override;

		  private:
			// Geometry pass.
			GLuint _fboGeo						 = GL_INVALID_VALUE;
			GLuint _colorNormalCompressedTexture = GL_INVALID_VALUE;
			GLuint _camSpacePositionsTexture	 = GL_INVALID_VALUE;
			GLuint _depthTexture				 = GL_INVALID_VALUE;

			GLuint _quadVAO = GL_INVALID_VALUE;
			GLuint _quadVBO = GL_INVALID_VALUE;

			// SSAO pass.
			GLuint				  _fboSSAO		  = GL_INVALID_VALUE;
			GLuint				  _ssaoTexture	  = GL_INVALID_VALUE;
			GLuint				  _noiseTexture	  = GL_INVALID_VALUE;
			Shader::GLSLProgram * _ssaoShader	  = nullptr;
			GLint				  _uProjMatrixLoc = GL_INVALID_INDEX;
			GLint				  _uAoKernelLoc	  = GL_INVALID_INDEX;
			Shader::GLSLProgram * _blurShader	  = nullptr;

			// Blur pass.
			GLuint _fboBlur		= GL_INVALID_VALUE;
			GLuint _blurTexture = GL_INVALID_VALUE;

			// Shading pass.
			GLuint				  _fboShading		 = GL_INVALID_VALUE;
			GLuint				  _shadingTexture	 = GL_INVALID_VALUE;
			Shader::GLSLProgram * _diffuseShading	 = nullptr;
			Shader::GLSLProgram * _toonShading		 = nullptr;
			Shader::GLSLProgram * _blinnPhongShading = nullptr;

			Shader::GLSLProgram * _currentShading = nullptr;

			// Anti-aliasing pass.
			Shader::GLSLProgram * _aaShader = nullptr;

			inline void _initGeometricPass();
			inline void _initSsaoPass();
			inline void _initBlurPass();
			inline void _initShadingPass();
			inline void _initAntiAliasingPass();
			inline void _initQuadVAO();

			inline void _geometricPass( Object3D::Scene & );
			inline void _ssaoPass( Object3D::Scene & );
			inline void _blurPass();
			inline void _shadingPass();
			inline void _antiAliasingPass();
		};
	} // namespace Renderer
} // namespace VTX

#endif
