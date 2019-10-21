/*
 * File: renderer.hpp
 * Author: Maxime MARIA
 */

#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "chrono.hpp"
#include "gBuffers.hpp"
#include "scene.hpp"

namespace vidocklab
{
	class Renderer
	{
	public:
		enum Shading
		{
			LAMBERT = 0,
			BLINN_PHONG,
			TOON
		};

		void init(const int width, const int height);
		double render(const Scene &scene);

		void setShading(const Shading &s)
		{
			switch (s)
			{
			case LAMBERT:
				m_currentShading = m_diffuseShading;
				break;
			case BLINN_PHONG:
				m_currentShading = m_blinnPhongShading;
				break;
			case TOON:
				m_currentShading = m_toonShading;
				break;
			default:
				m_currentShading = m_diffuseShading;
				break;
			}
		}

	private:
		inline double forwardRendering(const Scene &scene);

		inline double deferredShadingRendering(const Scene &scene);
		inline void geometricPass(const Scene &scene);
		inline void ssaoPass(const Scene &scene);
		inline void blurPass();
		inline void shadingPass();
		inline void antiAliasingPass();

	private:
		Chrono m_chrono;

		// geometry pass
		GLuint m_fboGeo = GL_INVALID_VALUE;
		GLuint m_colorNormalCompressedTexture = GL_INVALID_VALUE;
		GLuint m_camSpacePositionsTexture = GL_INVALID_VALUE;
		GLuint m_depthTexture;

		GLuint m_quadVAO = GL_INVALID_VALUE;
		GLuint m_quadVBO = GL_INVALID_VALUE;

		// ssao pass
		GLuint m_fboSSAO = GL_INVALID_VALUE;
		GLuint m_ssaoTexture = GL_INVALID_VALUE;
		GLuint m_noiseTexture = GL_INVALID_VALUE;
		GLSLProgram *m_ssaoShader = nullptr;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;
		GLint m_uAoKernelLoc = GL_INVALID_INDEX;
		GLSLProgram *m_blurShader = nullptr;

		// blur pass
		GLuint m_fboBlur = GL_INVALID_VALUE;
		GLuint m_blurTexture = GL_INVALID_VALUE;

		// shading pass
		GLuint m_fboShading = GL_INVALID_VALUE;
		GLuint m_shadingTexture = GL_INVALID_VALUE;
		GLSLProgram *m_diffuseShading = nullptr;
		GLSLProgram *m_toonShading = nullptr;
		GLSLProgram *m_blinnPhongShading = nullptr;

		GLSLProgram *m_currentShading = nullptr;

		// anti-aliasing pass
		GLSLProgram *m_aaShader = nullptr;
	};
} // namespace vidocklab

#endif // __RENDERER_HPP__
