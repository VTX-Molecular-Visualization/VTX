#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#include <util/types.hpp>
#include "app/old_app/generic/base_opengl.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/renderer/gl/program.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	class GL;
	namespace Pass
	{
		class BasePass : public Generic::BaseOpenGL
		{
		  public:
			BasePass()			= default;
			virtual ~BasePass() = default;

			virtual void init( const uint, const uint, const GL & )	   = 0;
			virtual void resize( const uint, const uint, const GL & )  = 0;
			virtual void render( const Object3D::Scene &, const GL & ) = 0;
		};
	} // namespace Pass
} // namespace VTX::Renderer::GL

#endif
