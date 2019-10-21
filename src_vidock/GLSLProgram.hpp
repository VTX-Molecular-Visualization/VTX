/*
 * File: GLSLProgram.hpp
 * Author: Maxime MARIA
 */

#ifndef __GLSL_PROGRAM_HPP__
#define __GLSL_PROGRAM_HPP__

#include <string>

#include "GL/gl3w.h"
#include "glm/glm.hpp"

#include "defines.hpp"
#include "io/filePath.hpp"

namespace vidocklab
{
	class GLSLProgram
	{
	public:
		GLSLProgram() = default;
		~GLSLProgram();

		inline GLuint getId() const;
		inline void use() const;

		void create(const std::string &name);
		void attachShader(const GLuint shaderId) const;
		void attachShader(const FilePath &shaderFilePath) const;
		void link();

	public:
		void detachShaders();
		std::string getInfoLog() const;

	private:
		std::string m_name = UNDEFINED_STRING;

		GLuint m_id = GL_INVALID_INDEX;
		// TODO remove
		/*GLuint m_vertShader = GL_INVALID_INDEX;
		GLuint m_fragShader = GL_INVALID_INDEX;
		GLuint m_geomShader = GL_INVALID_INDEX;
		GLuint m_compShader = GL_INVALID_INDEX;
		GLuint m_tescShader = GL_INVALID_INDEX;
		GLuint m_teseShader = GL_INVALID_INDEX;*/

		friend class GLSLProgramManager;
	};

	inline GLuint GLSLProgram::getId() const
	{
		return m_id;
	}

	inline void GLSLProgram::use() const
	{
		glUseProgram(m_id);
	}

} // namespace vidocklab

#endif // __GLSL_PROGRAM_HPP__
