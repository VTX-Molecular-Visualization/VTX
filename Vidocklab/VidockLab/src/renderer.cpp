/*
 * File: renderer.cpp
 * Author: Maxime MARIA
 */

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "io/ioUtils.hpp"

#include "app.hpp"
#include "renderer.hpp"

#include "ballAndStickRepDeferred.hpp"
#include "vdwSphereRepGeomShaderDeferred.hpp"

namespace vidocklab
{
	void Renderer::init(const int width, const int height)
	{
		std::cout << width << " - " << height << std::endl;
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.f);
		glDepthFunc(GL_LESS);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
		glViewport(0, 0, width, height);
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// create G-buffers for deferred shading
		glGenFramebuffers(1, &m_fboGeo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboGeo);

		glGenTextures(1, &m_colorNormalCompressedTexture);
		glBindTexture(GL_TEXTURE_2D, m_colorNormalCompressedTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &m_camSpacePositionsTexture);
		glBindTexture(GL_TEXTURE_2D, m_camSpacePositionsTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenTextures(1, &m_depthTexture);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 m_colorNormalCompressedTexture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							 m_camSpacePositionsTexture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							 m_depthTexture, 0);

		static const GLenum draw_buffers[]
			= { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers(2, draw_buffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ========================
		// SSAO
		// ========================

		glGenFramebuffers(1, &m_fboSSAO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboSSAO);

		glGenTextures(1, &m_ssaoTexture);
		glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 m_ssaoTexture, 0);
		static const GLenum draw_bufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufferSSAO);

		m_ssaoShader = App::get().createProgram("SSAO");
		m_ssaoShader->attachShader("shading/ssao.frag");
		m_ssaoShader->link();

		m_uProjMatrixLoc
			= glGetUniformLocation(m_ssaoShader->getId(), "uProjMatrix");
		m_uAoKernelLoc
			= glGetUniformLocation(m_ssaoShader->getId(), "uAoKernel");

		// generate random ao kernel
		std::random_device rd;
		static std::mt19937 gen(0); // rd());
		static std::uniform_real_distribution<float> dist(0.f, 1.f);
		const uint kernelSize = 32;
		Vec3f aoKernel[kernelSize];
		for (uint i = 0; i < kernelSize; i++)
		{
			std::cout << "adaptative ? " << std::endl;
			Vec3f v;
			// sample on unit hemisphere
			v.x = 2.f * dist(gen) - 1.f;
			v.y = 2.f * dist(gen) - 1.f;
			v.z = dist(gen);
			v = glm::normalize(v);
			// scale sample within the hemisphere
			v *= dist(gen);
			// accelerating interpolation (distance from center reduces when
			// number of points grow up)
			float scale = float(i) / float(kernelSize);
			scale = 0.1f + 0.9f * scale * scale; // lerp
			v *= scale;
			aoKernel[i] = v;
		}

		m_ssaoShader->use();
		glUniform3fv(m_uAoKernelLoc, kernelSize, (const GLfloat *)aoKernel);

		// generate noise texture
		const uint noiseTextureSize = 128;
		std::vector<Vec3f> noise(noiseTextureSize * noiseTextureSize);

		for (uint i = 0; i < noiseTextureSize * noiseTextureSize; ++i)
		{
			noise[i] = Vec3f(dist(gen), dist(gen), 0.f);
			noise[i] = glm::normalize(noise[i]);
		}
		glGenTextures(1, &m_noiseTexture);
		glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseTextureSize,
					 noiseTextureSize, 0, GL_RGB, GL_FLOAT, noise.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// repeat tile over the image
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// ========================
		// Blur
		// ========================
		glGenFramebuffers(1, &m_fboBlur);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboBlur);

		glGenTextures(1, &m_blurTexture);
		glBindTexture(GL_TEXTURE_2D, m_blurTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 m_blurTexture, 0);
		static const GLenum draw_bufferBlur[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufferBlur);

		m_blurShader = App::get().createProgram("Blur");
		m_blurShader->attachShader("shading/blur.frag");
		m_blurShader->link();

		// ========================
		// Shading
		// ========================
		glGenFramebuffers(1, &m_fboShading);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboShading);

		glGenTextures(1, &m_shadingTexture);
		glBindTexture(GL_TEXTURE_2D, m_shadingTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 m_shadingTexture, 0);
		static const GLenum draw_bufferShading[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufferShading);
		m_toonShading = App::get().createProgram("ToonShading");
		m_toonShading->attachShader("shading/toonShading.frag");
		m_toonShading->link();
		m_diffuseShading = App::get().createProgram("DiffuseShading");
		m_diffuseShading->attachShader("shading/diffuseShading.frag");
		m_diffuseShading->link();
		m_blinnPhongShading = App::get().createProgram("BlinnPhongShading");
		m_blinnPhongShading->attachShader("shading/blinnPhongShading.frag");
		m_blinnPhongShading->link();

		m_currentShading = m_diffuseShading;

		// ========================
		// AA
		// ========================
		m_aaShader = App::get().createProgram("AA");
		m_aaShader->attachShader("shading/fxaa.frag");
		m_aaShader->link();

		// ========================
		// QUAD FOR DEFERRED
		// ========================
		// init quad vao/vbo for deferred shading

		// clang-format off
		GLfloat m_quadVertices[] =
		{
			-1.0f,  1.0f,
            -1.0f, -1.0f, 
             1.0f,  1.0f,
             1.0f, -1.0f,
		};
		// clang-format on

		// setup plane VAO
		glGenBuffers(1, &m_quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_quadVertices), m_quadVertices,
					 GL_STATIC_DRAW);
		glGenVertexArrays(1, &m_quadVAO);
		glBindVertexArray(m_quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	double Renderer::render(const Scene &scene)
	{
		// return forwardRendering(scene);
		return deferredShadingRendering(scene);
	}

	inline double Renderer::forwardRendering(const Scene &scene)
	{
		m_chrono.start();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (uint i = 0; i < AMoleculeRep::RepType::NB_REP_TYPES; ++i)
		{
			const AMoleculeRep *rep = scene.m_moleculeReps[i];
			/// TODO : this is uggly
			if (rep != nullptr && rep->isVisible())
			{
				scene.m_molecule->bindVAO();

				rep->use();
				rep->setCameraMatrices(*(scene.m_camera));

				rep->draw(0, scene.m_molecule->m_nbAtoms);
				scene.m_molecule->bindVAO();
			}
		}

		m_chrono.stop();
		return m_chrono.elapsedTime();
	}

	inline double Renderer::deferredShadingRendering(const Scene &scene)
	{
		m_chrono.start();

		glEnable(GL_DEPTH_TEST);
		geometricPass(scene);

		glDisable(GL_DEPTH_TEST);
		ssaoPass(scene);

		blurPass();

		shadingPass();

		// antiAliasingPass();

		m_chrono.stop();
		return m_chrono.elapsedTime();
	}

	inline void Renderer::geometricPass(const Scene &scene)
	{
		// geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboGeo);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
				| GL_STENCIL_BUFFER_BIT);

		const VdwSphereRepGeomShaderDeferred *repVDW
			= dynamic_cast<VdwSphereRepGeomShaderDeferred *>(
				scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER_DEFERRED]);
		const BallAndStickRepDeferred *repBS
			= dynamic_cast<BallAndStickRepDeferred *>(
				scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER_DEFERRED]);

		scene.m_molecule->bindVAO();
		scene.m_molecule->bindIBO();
		if (repVDW != nullptr && repVDW->isVisible())
		{
			repVDW->use();
			if (repBS != nullptr && repBS->isVisible())
			{
				repVDW->setRadScale(0.3f);
			}
			else
			{
				repVDW->setRadScale(1.f);
			}
			repVDW->setCameraMatrices(*(scene.m_camera));
			repVDW->draw(0, scene.m_molecule->m_nbAtoms);
		}
		if (repBS != nullptr && repBS->isVisible())
		{
			repBS->use();
			repBS->setCameraMatrices(*(scene.m_camera));
			repBS->draw(0, scene.m_molecule->m_nbBonds);
		}
		scene.m_molecule->unbindIBO();
		scene.m_molecule->unbindVAO();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline void Renderer::ssaoPass(const Scene &scene)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboSSAO);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorNormalCompressedTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_camSpacePositionsTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);

		m_ssaoShader->use();

		glUniformMatrix4fv(
			m_uProjMatrixLoc, 1, GL_FALSE,
			glm::value_ptr((*(scene.m_camera)).getProjectionMatrix()));

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		// TODO Needed ?
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	inline void Renderer::blurPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboBlur);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);

		m_blurShader->use();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		// TODO Needed ?
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	inline void Renderer::shadingPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glBindFramebuffer(GL_FRAMEBUFFER, m_fboShading);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorNormalCompressedTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_camSpacePositionsTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_blurTexture);

		m_currentShading->use();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	inline void Renderer::antiAliasingPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_shadingTexture);

		m_aaShader->use();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
} // namespace vidocklab
