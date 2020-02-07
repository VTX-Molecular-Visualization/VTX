#ifndef __VTX_RENDERER_GL__
#define __VTX_RENDERER_GL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class RendererGL : public BaseRenderer
		{
		  public:
			RendererGL()  = default;
			~RendererGL() = default;

			virtual void init( Object3D::Scene &, uint, uint ) override;
			virtual void clear( Object3D::Scene & ) override;
			virtual void render( Object3D::Scene & ) override;
			virtual void setShading() override;

		  private:
			// Geometry pass.
			GLuint _fboGeo						 = GL_INVALID_VALUE;
			GLuint _colorNormalCompressedTexture = GL_INVALID_VALUE;
			GLuint _camSpacePositionsTexture	 = GL_INVALID_VALUE;
			GLuint _depthTexture				 = GL_INVALID_VALUE;

			GLuint _quadVAO = GL_INVALID_VALUE;
			GLuint _quadVBO = GL_INVALID_VALUE;

			// SSAO pass.
			GL::GLSLProgram * _ssaoShader		= nullptr;
			GLuint			  _fboSSAO			= GL_INVALID_VALUE;
			GLuint			  _ssaoTexture		= GL_INVALID_VALUE;
			GLuint			  _noiseTexture		= GL_INVALID_VALUE;
			GLint			  _uProjMatrixLoc	= GL_INVALID_INDEX;
			GLint			  _uAoKernelLoc		= GL_INVALID_INDEX;
			GLint			  _uAoRadiusLoc		= GL_INVALID_INDEX;
			GLint			  _uKernelSizeLoc	= GL_INVALID_INDEX;
			GLint			  _uAoIntensityLoc	= GL_INVALID_INDEX;
			GLint			  _kernelSize		= 32;
			GLuint			  _noiseTextureSize = 9;

			// Blur pass.
			GL::GLSLProgram * _blurShader	= nullptr;
			GLuint			  _fboBlur		= GL_INVALID_VALUE;
			GLuint			  _blurTexture	= GL_INVALID_VALUE;
			GLint			  _uBlurSizeLoc = GL_INVALID_INDEX;

			// Shading pass.
			GL::GLSLProgram * _diffuseShading	 = nullptr;
			GL::GLSLProgram * _blinnPhongShading = nullptr;
			GL::GLSLProgram * _toonShading		 = nullptr;
			GLuint			  _fboShading		 = GL_INVALID_VALUE;
			GLuint			  _shadingTexture	 = GL_INVALID_VALUE;

			GL::GLSLProgram * _currentShading = nullptr;

			// Anti-aliasing pass.
			GL::GLSLProgram * _aaShader = nullptr;

			void _initGeometricPass();
			void _initSsaoPass();
			void _initBlurPass();
			void _initShadingPass();
			void _initAntiAliasingPass();
			void _initQuadVAO();

			void _geometricPass( Object3D::Scene & );
			void _ssaoPass( Object3D::Scene & );
			void _blurPass();
			void _shadingPass();
			void _antiAliasingPass();
		};
	} // namespace Renderer
} // namespace VTX

#endif
