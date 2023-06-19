#ifndef __VTX_RENDERER_GL_PASS_BASE_PASS__
#define __VTX_RENDERER_GL_PASS_BASE_PASS__

#include "renderer/gl/include_opengl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "renderer/gl/vertex_array.hpp"
#include <concepts>

namespace VTX::Renderer::GL::Pass
{

	template<typename T>
	concept Renderable = requires( T				p_pass,
								   const size_t		p_width,
								   const size_t		p_height,
								   ProgramManager & p_pm,
								   VertexArray &	p_vao ) {
		{
			p_pass.init( p_width, p_height, p_pm )
		} -> std::same_as<void>;
		{
			p_pass.resize( p_width, p_height )
		} -> std::same_as<void>;
		{
			p_pass.render( p_vao )
		} -> std::same_as<void>;
	};

	template<Renderable R>
	class BasePass : public R
	{
	  public:
		GLuint queryStart;
		GLuint queryEnd;

		BasePass()
		{
			glGenQueries( 1, &queryStart );
			glGenQueries( 1, &queryEnd );
		}

		~BasePass()
		{
			glDeleteQueries( 1, &queryStart );
			glDeleteQueries( 1, &queryEnd );
		}
	};

} // namespace VTX::Renderer::GL::Pass

#endif
