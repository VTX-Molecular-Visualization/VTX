/*
 * File: gBuffers.hpp
 * Author: Maxime MARIA
 */

#ifndef __G_BUFFERS_HPP__
#define __G_BUFFERS_HPP__

#include "GL/gl3w.h"

#include "defines.hpp"

namespace vidocklab
{
	class GBuffers
	{
	public:
		enum GbuffersTextureType
		{
			GBT_COLOR_NORMAL_COMPRESSED = 0,
			GBT_CAM_POSITION,
			GBT_NB_TYPES
		};

		GBuffers() = default;
		~GBuffers();

		void init(const uint width, const uint height);

		void bindFBO() const;
		void unbindFBO() const;

	public:
		GLuint m_fbo = GL_INVALID_VALUE;
		GLuint m_textures[GBT_NB_TYPES] = { GL_INVALID_VALUE };
		GLuint m_depthTexture = GL_INVALID_VALUE;
	};

} // namespace vidocklab

#endif // __G_BUFFERS_HPP__
