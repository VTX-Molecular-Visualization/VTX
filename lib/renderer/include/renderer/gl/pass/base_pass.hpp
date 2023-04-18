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

		virtual void init( const uint p_width, const uint p_height )   = 0;
		virtual void resize( const uint p_width, const uint p_height ) = 0;
		virtual void render()										   = 0;
	};

} // namespace VTX::Renderer::GL::Pass

#endif
