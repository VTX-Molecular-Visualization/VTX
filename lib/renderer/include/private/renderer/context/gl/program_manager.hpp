#ifndef __VTX_RENDERER_CONTEXT_GL_PROGRAM_MANAGER__
#define __VTX_RENDERER_CONTEXT_GL_PROGRAM_MANAGER__

#include "buffer.hpp"
#include "enum_shader_type.hpp"
#include "program.hpp"
#include <map>
#include <variant>
#include <vector>

namespace VTX
{
	namespace Renderer::Context::GL
	{

		class ProgramManager
		{
		  public:
			explicit ProgramManager( const FilePath & p_shaderPath );
			~ProgramManager();

			static ENUM_SHADER_TYPE getShaderType( const FilePath & );

			Program * const createProgram(
				const std::string & p_name,
				const std::variant<FilePath, std::vector<FilePath>> &,
				const std::string & p_toInject = "",
				const std::string & p_suffix   = ""
			);

			void			deleteProgram( const std::string & );
			Program * const getProgram( const std::string & p_name );
			void			compileShaders();
			uint32_t		getShader( const std::string & p_name ) const;

			void dispose();

		  private:
			using MapStringToProgram = std::map<std::string, std::unique_ptr<Program>>;
			using MapStringTouint32_t	 = std::map<std::string, uint32_t>;

			static const std::map<std::string, ENUM_SHADER_TYPE> _EXTENSIONS;

			const FilePath	   _shaderPath;
			MapStringToProgram _programs = MapStringToProgram();
			MapStringTouint32_t  _shaders	 = MapStringTouint32_t();

			uint32_t	_createShader( const FilePath &, const std::string & = "", const std::string & = "" );
			std::string _getShaderErrors( const uint32_t );
		};
	} // namespace Renderer::Context::GL
} // namespace VTX

#endif
