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
			ProgramManager( const FilePath & p_rootPath = std::filesystem::current_path() );
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
			using MapStringToProgram = std::map<std::string, std::unique_ptr<Program>>;
			using MapStringToGLuint	 = std::map<std::string, GLuint>;

			static const std::map<std::string, ENUM_SHADER_TYPE> _EXTENSIONS;

			const FilePath	   _rootPath;
			MapStringToProgram _programs = MapStringToProgram();
			MapStringToGLuint  _shaders	 = MapStringToGLuint();

			GLuint		_createShader( const FilePath &, const std::string & = "", const std::string & = "" );
			std::string _getShaderErrors( const GLuint );
		};
	} // namespace Renderer::GL
} // namespace VTX

#endif
