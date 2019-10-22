/*
 * File: GLSLProgram.cpp
 * Author: Maxime MARIA
 */

#include "GLSLProgram.hpp"
#include "GLSLProgramManager.hpp"

#include <vector>

#include "exceptions.hpp"
#include "io/ioUtils.hpp"

namespace vidocklab
{
	GLSLProgram::~GLSLProgram()
	{
		if (m_id != GL_INVALID_INDEX)
		{
			// detach but don't delete shaders, can be used by other programs
			detachShaders();
			glDeleteProgram(m_id);
		}
	}

	void GLSLProgram::create(const std::string &name)
	{
		if (m_id == GL_INVALID_INDEX)
		{
			m_name = name;
			m_id = glCreateProgram();
		}
	}

	void GLSLProgram::attachShader(const GLuint shaderId) const
	{
		if (m_id == GL_INVALID_INDEX)
			throw GLSLException("cannot attach shader: program is not created");

		glAttachShader(m_id, shaderId);
	}

	void GLSLProgram::attachShader(const FilePath &shaderFilePath) const
	{
		if (m_id == GL_INVALID_INDEX)
			throw GLSLException("cannot attach shader: program is not created");

		GLSLProgramManager &progManager = GLSLProgramManager::get();
		const GLuint shaderId = progManager.createShader(shaderFilePath);
		attachShader(shaderId);
	}

	void GLSLProgram::link()
	{
		VerboseOStream::cout << "Linking program " << m_name << std::endl;

		if (m_id == GL_INVALID_INDEX)
			throw GLSLException("cannot link program: program is not created");

		GLint linked;
		glLinkProgram(m_id);
		glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			std::string err = "error linking program: ";
			err += m_name;
			err += "\n";
			err += getInfoLog();
			glDeleteProgram(m_id);
			throw GLSLException(err);
		}
	}

	void GLSLProgram::detachShaders()
	{
		GLint nbShaders = 0;
		glGetProgramiv(m_id, GL_ATTACHED_SHADERS, &nbShaders);
		std::vector<GLuint> shaders(nbShaders);
		glGetAttachedShaders(m_id, nbShaders, nullptr, shaders.data());
		for (int i = 0; i < shaders.size(); ++i)
		{
			glDetachShader(m_id, shaders[i]);
		}
	}

	std::string GLSLProgram::getInfoLog() const
	{
		GLint length;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetProgramInfoLog(m_id, length, &length, &(log[0]));
		return std::string(log.begin(), log.end());
	}
} // namespace vidocklab
