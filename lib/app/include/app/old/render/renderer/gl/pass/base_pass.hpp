#ifndef __VTX_RENDER_RENDERER_GL_PASS_BASE_PASS__
#define __VTX_RENDER_RENDERER_GL_PASS_BASE_PASS__

#include "app/old/application/scene.hpp"
#include "app/old/render/generic/base_opengl.hpp"
#include "app/old/render/renderer/gl/program.hpp"
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Render::Renderer::GL
{
	class GL;
	namespace Pass
	{
		class BasePass : public Render::Generic::BaseOpenGL
		{
		  public:
			BasePass()			= default;
			virtual ~BasePass() = default;

			virtual void init( const uint, const uint, const GL & )			   = 0;
			virtual void resize( const uint, const uint, const GL & )		   = 0;
			virtual void render( const App::Old::Application::Scene &, const GL & ) = 0;
		};
	} // namespace Pass
} // namespace VTX::App::Old::Render::Renderer::GL

#endif
