#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept_context.hpp"

namespace VTX::Renderer::Context
{
	class OpenGL45
	{
	  public:
		void add( const DescAttachment & p_desc ) {}
		void add( const DescStorage & p_desc ) {}
		void add( const DescProgram & p_desc ) {}

	  private:
	};
} // namespace VTX::Renderer::Context

#endif
