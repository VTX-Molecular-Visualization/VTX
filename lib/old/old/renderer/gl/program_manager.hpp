#ifndef __VTX_GL_PROGRAM_MANAGER__
#define __VTX_GL_PROGRAM_MANAGER__

#include <util/types.hpp>
#include "generic/base_opengl.hpp"
#include "program.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Renderer::GL
	{
		enum class SHADER_TYPE : GLuint
		{
			VERTEX			= GL_VERTEX_SHADER,
			FRAGMENT		= GL_FRAGMENT_SHADER,
			GEOMETRY		= GL_GEOMETRY_SHADER,
			COMPUTE			= GL_COMPUTE_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
			TESS_CONTROL	= GL_TESS_CONTROL_SHADER,
			INVALID			= GL_INVALID_VALUE
		};

		class ProgramManager : public Generic::BaseOpenGL
		{
		  public:
			using MapStringToEnum	  = std::map<std::string, SHADER_TYPE>;
			using MapStringToProgram  = std::map<std::string, Program *>;
			using PairStringToProgram = std::pair<const std::string, Program *>;
			using MapStringToGLuint	  = std::map<std::string, GLuint>;
			using PairStringToGLuint  = std::pair<const std::string, GLuint>;

			inline static ProgramManager & get()
			{
				static ProgramManager instance;
				return instance;
			}

			Program * const createProgram( const std::string &,
										   const std::vector<FilePath> &,
										   const std::string & = "",
										   const std::string & = "" );
			void			deleteProgram( const std::string & );
			Program * const getProgram( const std::string & );
			void			refreshShaders();
			GLuint			getShader( const std::string & ) const;

			void dispose();

			static const MapStringToEnum EXTENSIONS;
			static SHADER_TYPE			 getShaderType( const FilePath & );

		  private:
			MapStringToProgram _programs = MapStringToProgram();
			MapStringToGLuint  _shaders	 = MapStringToGLuint();

			ProgramManager() = default;
			~ProgramManager();

			GLuint		_createShader( const FilePath &, const std::string & = "", const std::string & = "" );
			std::string _getShaderErrors( const GLuint );
		};
	} // namespace Renderer::GL

	inline Renderer::GL::ProgramManager & VTX_PROGRAM_MANAGER() { return Renderer::GL::ProgramManager::get(); }
} // namespace VTX

#endif
