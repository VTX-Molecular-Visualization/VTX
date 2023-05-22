#ifndef __VTX_RENDERER_GL_PASS_BASE_PASS__
#define __VTX_RENDERER_GL_PASS_BASE_PASS__

#include "renderer/gl/include_opengl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "renderer/gl/vertex_array.hpp"

namespace VTX::Renderer::GL::Pass
{

	class BasePass
	{
	  public:
		BasePass()			= default;
		virtual ~BasePass() = default;

		virtual void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) {}
		virtual void resize( const size_t p_width, const size_t p_height ) {}
		virtual void render( VertexArray & p_vao ) = 0;

		/*
		struct StructIn
		{
			// Input data as raw pointers.
		} in;

		struct StructOut
		{
			// Output data.
		} out;
		*/
	};

} // namespace VTX::Renderer::GL::Pass

#endif
