#ifndef __VTX_RENDERER_GL_PROGRAM_MANAGER__
#define __VTX_RENDERER_GL_PROGRAM_MANAGER__

#include "enum_shader_type.hpp"
#include "program.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Renderer::GL
	{

		class ProgramManager
		{
		  public:
			ProgramManager() = default;
			~ProgramManager();

			static ENUM_SHADER_TYPE getShaderType( const FilePath & );

			Program * const createProgram( const std::string &			 p_name,
										   const std::vector<FilePath> & p_shaders,
										   const std::string &			 p_toInject = "",
										   const std::string &			 p_suffix	= "" );
			void			deleteProgram( const std::string & );
			Program * const getProgram( const std::string & p_name );
			void			refreshShaders();
			GLuint			getShader( const std::string & p_name ) const;
			void			dispose();

		  private:
			using MapStringToEnum	  = std::map<std::string, ENUM_SHADER_TYPE>;
			using MapStringToProgram  = std::map<std::string, Program *>;
			using PairStringToProgram = std::pair<const std::string, Program *>;
			using MapStringToGLuint	  = std::map<std::string, GLuint>;
			using PairStringToGLuint  = std::pair<const std::string, GLuint>;

			static const MapStringToEnum _EXTENSIONS;

			MapStringToProgram _programs = MapStringToProgram();
			MapStringToGLuint  _shaders	 = MapStringToGLuint();

			GLuint		_createShader( const FilePath &, const std::string & = "", const std::string & = "" );
			std::string _getShaderErrors( const GLuint );
		};
	} // namespace Renderer::GL
} // namespace VTX

#endif
