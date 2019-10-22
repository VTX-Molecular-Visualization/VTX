/*
 * File: geometryPass.hpp
 * Author: Maxime MARIA
 */

#ifndef __GEOMETRY_PASS_HPP__
#define __GEOMETRY_PASS_HPP__

#include "GL/gl3w.h"

#include "defines.hpp"

namespace vidocklab
{
	class GeometryPass // : public ARenderPass
	{
	public:
		GeometryPass() = default;

		void init(const uint width, const uint height);

	private:
		GLuint m_fbo;
		GLuint m_colorNormalCompressedTexture = GL_INVALID_VALUE;
		GLuint m_camSpacePositionsTexture = GL_INVALID_VALUE;
		GLuint m_depthTexture = GL_INVALID_VALUE;
	};

} // namespace vidocklab

#endif // __GEOMETRY_PASS_HPP__
