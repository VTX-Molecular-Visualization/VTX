#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_cleanable.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/program_manager.hpp"
#include <GL/gl3w.h>

namespace VTX
{
	namespace Renderer
	{
		class GL;

		namespace Pass
		{
			class BasePass : public Generic::BaseCleanable
			{
			  public:
				BasePass() = default;
				virtual ~BasePass() {};

				virtual void init( GLSL::ProgramManager &, const uint, const uint )	 = 0;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) = 0;

			  private:
			};
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX

#endif
