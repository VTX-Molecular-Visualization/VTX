#ifndef __VTX_RENDERER_GL_PASS_BASE_PASS__
#define __VTX_RENDERER_GL_PASS_BASE_PASS__

#include "renderer/gl/include_opengl.hpp"

namespace VTX::Renderer::GL::Pass
{

	class BasePass
	{
	  public:
		BasePass()			= default;
		virtual ~BasePass() = default;

		virtual void init( const size_t p_width, const size_t p_height ) {}
		virtual void resize( const size_t p_width, const size_t p_height ) {}
		virtual void render() = 0;

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
