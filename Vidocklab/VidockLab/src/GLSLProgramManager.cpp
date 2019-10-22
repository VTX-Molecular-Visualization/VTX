/*
 * File: GLSLProgramManager.cpp
 * Author: Maxime MARIA
 */

#include "GLSLProgramManager.hpp"

#include <iostream>
#include <vector>

#include "app.hpp"
#include "defines.hpp"
#include "exceptions.hpp"
#include "io/ioUtils.hpp"

namespace vidocklab
{
	template<>
	GLSLProgramManager *Singleton<GLSLProgramManager>::m_instance = nullptr;

	GLSLProgramManager::GLSLProgramManager()
		: m_extensions({ { ".vert", GLSLShader::Type::VERTEX },
						 { ".geom", GLSLShader::Type::GEOMETRY },
						 { ".frag", GLSLShader::Type::FRAGMENT },
						 { ".comp", GLSLShader::Type::COMPUTE },
						 { ".tesc", GLSLShader::Type::TESS_CONTROL },
						 { ".tese", GLSLShader::Type::TESS_EVALUATION } })
	{
	}

	GLSLProgramManager::~GLSLProgramManager()
	{
		// delete shaders
		for (MapShaders::const_iterator it = m_shaders.begin();
			 it != m_shaders.end(); it++)
		{
			glDeleteShader(it->second);
		}
	}

	GLSLProgram *GLSLProgramManager::createProgram(const std::string &name)
	{
		VerboseOStream::cout << "Create program " << name << std::endl;

		if (m_programs.find(name) == m_programs.end())
		{
			m_programs[name] = GLSLProgram();
			m_programs[name].create(name);
		}
		else
		{
			VerboseOStream::cout << "-> already exists" << std::endl;
		}
		return &m_programs[name];
	}

	void GLSLProgramManager::deleteProgram(const std::string &name)
	{
		MapPrograms::const_iterator it = m_programs.find(name);
		if (it != m_programs.end())
			m_programs.erase(it);
	}

	const GLSLProgram &GLSLProgramManager::getProgram(
		const std::string &name) const
	{
		MapPrograms::const_iterator it = m_programs.find(name);
		if (it == m_programs.end())
			throw GLSLException("program does not exist");
		return it->second;
	}

	// TODO remove, useless
	std::string GLSLProgramManager::getProgramsInfoStr()
	{
		std::stringstream sstr;
		const size_t nbPgms = m_programs.size();
		if (nbPgms == 0)
		{
			sstr << "No program" << std::endl;
		}
		else
		{
			int cur = 1;
			for (MapPrograms::const_iterator itPgm = m_programs.begin();
				 itPgm != m_programs.end(); itPgm++)
			{
				const GLSLProgram &pgm = itPgm->second;
				sstr << cur++ << "/" << nbPgms << ": " << pgm.m_name
					 << std::endl;
			}
		}
		return sstr.str();
	}

	GLuint GLSLProgramManager::createShader(const FilePath &shaderPath)
	{
		VerboseOStream::cout << "--- Create shader: " << shaderPath
							 << std::endl;

		// get extension
		const std::string name = shaderPath.file();
		const GLSLShader::Type type = getShaderType(name);
		if (type == GLSLShader::Type::INVALID)
			throw GLSLException("invalid shader extension:" + name);

		VerboseOStream::cout << "---> type: " << shaderTypeToString(type)
							 << std::endl;

		GLuint shaderId = getShader(name);
		if (shaderId == COMPILE_SHADER)
		{
			VerboseOStream::cout << "---> compiling" << std::endl;
			// create shader
			shaderId = glCreateShader(type);
			// read code from file
			const std::string str
				= readFile(AppConfig::shadersDirPath + shaderPath);

			const GLchar *shaderCode = str.c_str();
			glShaderSource(shaderId, 1, &shaderCode, 0);
			// compile shader
			glCompileShader(shaderId);
			// check if compile is ok
			GLint compiled;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
			if (!compiled)
			{
				std::string err = "error compiling shader: ";
				err += name;
				err += "\n";
				err += getShaderErrors(shaderId);
				glDeleteShader(shaderId);
				throw GLSLException(err);
			}
			m_shaders[name] = shaderId;
			VerboseOStream::cout << "---> " << DONE_STRING << std::endl;
		}
		else
		{
			VerboseOStream::cout << "---> already exists !" << std::endl;
		}
		return shaderId;
	}

	GLuint GLSLProgramManager::getShader(const std::string &name)
	{
		MapShaders::const_iterator it = m_shaders.find(name);

		return it == m_shaders.end() ? COMPILE_SHADER : it->second;
	}

	GLSLShader::Type GLSLProgramManager::getShaderType(
		const std::string &name) const
	{
		const size_t pos = name.find_first_of('.');
		if (pos == std::string::npos)
		{
			return GLSLShader::Type::INVALID;
		}
		MapExtensions::const_iterator it = m_extensions.find(name.substr(pos));

		return it == m_extensions.end() ? GLSLShader::Type::INVALID
										: it->second;
	}

	std::string GLSLProgramManager::shaderTypeToString(
		const GLSLShader::Type type) const
	{
		std::string str;
		switch (type)
		{
		case GLSLShader::Type::VERTEX:
			str = "vertex";
			break;
		case GLSLShader::Type::FRAGMENT:
			str = "fragment";
			break;
		case GLSLShader::Type::GEOMETRY:
			str = "geometry";
			break;
		case GLSLShader::Type::COMPUTE:
			str = "compute";
			break;
		case GLSLShader::Type::TESS_CONTROL:
			str = "tessellation control";
			break;
		case GLSLShader::Type::TESS_EVALUATION:
			str = "tessellation evaluation";
			break;
		default:
			str = "invalid";
			break;
		}
		return str;
	}

	std::string GLSLProgramManager::getShaderErrors(const GLuint shader)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetShaderInfoLog(shader, length, &length, &(log[0]));
		return std::string(log.begin(), log.end());
	}

	std::string GLSLProgramManager::getShaderNameFromId(const GLuint shader)
	{
		MapShaders::const_iterator itShader
			= std::find_if(m_shaders.begin(), m_shaders.end(),
						   [shader](const std::pair<std::string, GLuint> &p) {
							   return p.second == shader;
						   });
		return itShader != m_shaders.end() ? itShader->first : UNDEFINED_STRING;
	}
} // namespace vidocklab
