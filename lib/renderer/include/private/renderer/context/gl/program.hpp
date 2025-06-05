#ifndef __VTX_RENDERER_CONTEXT_GL_PROGRAM__
#define __VTX_RENDERER_CONTEXT_GL_PROGRAM__

#include "include_opengl.hpp"
#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Context::GL
{
	class Program
	{
	  public:
		Program() = default;
		explicit Program( const std::vector<FilePath> & p_shaderPaths, const std::string & p_toInject = "" ) :
			_shaderPaths( p_shaderPaths ), _toInject( p_toInject )
		{
		}
		~Program();

		inline const uint32_t				 getId() const { return _id; }
		inline void							 setId( const uint32_t p_id ) { _id = p_id; }
		inline const std::vector<FilePath> & getShaderPaths() const { return _shaderPaths; }
		inline const std::string &			 getToInject() const { return _toInject; }

		inline void use() const { glUseProgram( _id ); }

		void create( const std::string & );
		void attachShader( const uint32_t );
		void link();
		void detachShaders();

	  private:
		uint32_t					_id			 = GL_INVALID_INDEX;
		std::string					_name		 = "";
		const std::vector<FilePath> _shaderPaths = std::vector<FilePath>();
		const std::string			_toInject	 = "";
		std::string					_getProgramErrors();

		friend class ProgramManager;
	};
} // namespace VTX::Renderer::Context::GL

#endif
