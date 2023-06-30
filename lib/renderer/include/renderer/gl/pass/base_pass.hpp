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
			T( p_width, p_height, p_pm )
		} -> std::same_as<T>;
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
		BasePass() = delete;
		BasePass( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) : R( p_width, p_height, p_pm ) {}
	};

} // namespace VTX::Renderer::GL::Pass

#endif
