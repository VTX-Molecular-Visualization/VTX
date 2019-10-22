/*
 * File: gBuffers.cpp
 * Author: Maxime MARIA
 */

#include "gBuffers.hpp"
#include "exceptions.hpp"

namespace vidocklab
{
	GBuffers::~GBuffers()
	{
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(GBT_NB_TYPES, m_textures);
		glDeleteTextures(1, &m_depthTexture);
	}

	void GBuffers::init(const uint width, const uint height)
	{
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(2, m_textures);

		glBindTexture(GL_TEXTURE_2D, m_textures[GBT_COLOR_NORMAL_COMPRESSED]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, GBT_CAM_POSITION);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &m_depthTexture);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 m_textures[GBT_COLOR_NORMAL_COMPRESSED], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							 m_textures[GBT_CAM_POSITION], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							 m_depthTexture, 0);

		const GLenum draw_buffers[]
			= { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers(2, draw_buffers);

		/*GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (err != GL_FRAMEBUFFER_COMPLETE)
		{
			throw GLException("Frame buffer error: " + err);
		}*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBT_COLOR_NORMAL_COMPRESSED]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBT_CAM_POSITION]);
	}

	void GBuffers::bindFBO() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void GBuffers::unbindFBO() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} // namespace vidocklab
