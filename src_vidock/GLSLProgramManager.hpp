/*
 * File: GLSLProgramManager.hpp
 * Author: Maxime MARIA
 */

#ifndef __GLSL_PROGRAM_MANAGER__
#define __GLSL_PROGRAM_MANAGER__

#include <map>

#include "io/filePath.hpp"
#include "singletonClass.hpp"

#include "GLSLProgram.hpp"

namespace vidocklab
{
	namespace GLSLShader
	{
		enum Type
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
			COMPUTE = GL_COMPUTE_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
			TESS_CONTROL = GL_TESS_CONTROL_SHADER,
			INVALID = GL_INVALID_VALUE
		};
	}

	class GLSLProgramManager : public Singleton<GLSLProgramManager>
	{
	public:
		GLSLProgramManager();
		~GLSLProgramManager();

		GLSLProgram *createProgram(const std::string &name);

		void deleteProgram(const std::string &name);

		const GLSLProgram &getProgram(const std::string &name) const;

		std::string getProgramsInfoStr();
		GLSLShader::Type getShaderType(const std::string &name) const;

		GLuint createShader(const FilePath &shaderPath);

	private:
		GLuint getShader(const std::string &name);
		std::string shaderTypeToString(const GLSLShader::Type type) const;

		std::string getShaderErrors(const GLuint shader);

		std::string getShaderNameFromId(const GLuint shader);

	private:
		GLuint m_currentProgram = GL_INVALID_INDEX;

		using MapPrograms = std::map<std::string, GLSLProgram>;
		MapPrograms m_programs;

		using MapShaders = std::map<std::string, GLuint>;
		MapShaders m_shaders;

		using MapExtensions = std::map<std::string, GLSLShader::Type>;
		const MapExtensions m_extensions;

		const GLuint COMPILE_SHADER = GL_INVALID_INDEX;
	};
} // namespace vidocklab

#endif // __GLSL_PROGRAM_MANAGER__
