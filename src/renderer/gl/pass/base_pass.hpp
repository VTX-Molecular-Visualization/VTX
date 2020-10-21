#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/program_manager.hpp"

namespace VTX
{
	namespace Renderer
	{
		class GL;

		namespace Pass
		{
			class BasePass : public Generic::BaseOpenGL
			{
			  public:
				BasePass( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
				virtual ~BasePass() = default;

				virtual void init( GLSL::ProgramManager &, const uint, const uint )	 = 0;
				virtual void resize( const uint, const uint )						 = 0;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) = 0;
			};
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX

#endif
