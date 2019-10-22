/*
 * File: aRenderPass.hpp
 * Author: Maxime MARIA
 */

#ifndef __A_RENDER_PASS_HPP__
#define __A_RENDER_PASS_HPP__

#include "GLSLProgram.hpp"

namespace vidocklab
{
	class GeometryPass // : public ARenderPass
	{
	public:
		GeometryPass() = default;

		void init();

	private:
		GLuint m_uCamSpacePosTextLoc = GL_INVALID_VALUE;
		GLuint m_uSampleRadiusLoc = GL_INVALID_VALUE;
		GLuint m_uAoKernelLoc = GL_INVALID_VALUE;
		GLuint m_uProjMatLoc = GL_INVALID_VALUE;
	};

} // namespace vidocklab

#endif // __A_RENDER_PASS_HPP__
