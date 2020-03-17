#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "object3d/scene.hpp"
#include "renderer/glsl/program_manager.hpp"
#include <GL/gl3w.h>

namespace VTX
{
	namespace Renderer
	{
		class GL;

		namespace Pass
		{
			class BasePass
			{
			  public:
				BasePass()			= default;
				virtual ~BasePass() = default;

				virtual void init( GLSL::ProgramManager &, const uint, const uint )	 = 0;
				virtual void clear()												 = 0;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) = 0;

			  private:
			};
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX

#endif
